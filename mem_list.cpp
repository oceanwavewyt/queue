#include "mem_list.h"
#include "file_env.h"
#include "reader.h"
#include "writer.h"
#include "version.h"

namespace pile {
	
	MemList *MemList::instance_[] = {NULL};

	MemList *MemList::Instance(const uint8_t level)
	{
		if(level>levelNum) return NULL;
		if(instance_[0]) return instance_[level];
		for(int i=0; i<=levelNum; i++){
		//if(instance_[level]) return instance_[level];
			instance_[i] = new MemList();
		}
		return instance_[level];
	}

	MemList::MemList():head_(NULL),tail_(NULL),currentMem_(0),currentReadFid_(0) {
		length_ = 0;
		writer_ = NULL;
		reader_ = NULL;
	}

	MemList::~MemList(){
		delete writer_;
	}

	void MemList::SetFilelist(FixFile *f) {
		filelist_ = f;
	}


	void MemList::SetWriter(string &filename) {
		if(writer_) delete writer_;
		MmapFile *mfile;
		Files f;
		if(!f.NewWritableFile(filename, &mfile)) {
			cout << filename << " failed NewWritableFile" << endl;
			exit(1);
		}	
		writer_ = new Writer(mfile);
	}

	void MemList::WriteRecord(const string &str, size_t length, uint8_t level) {
		//cout << "blockid: "<< Version::Instance()->GetBlockId() << endl;	
		if(Version::Instance()->GetBlockId() >= fMaxBlockNum) {
			cout << "start release..................." << endl; 
			filelist_->ReleaseCurFile();
			Version::Instance()->Init(0,0);
			string filename;
			filelist_->GetCurrentFile(filename);
			SetWriter(filename);	
		}
		cout << "write: "<< str << endl;
		writer_->AddRecord(str, length);
	}

	void MemList::Push(QueueItem *item) 
	{
		QueueLink *curit = new QueueLink(item); 
		if(tail_ != NULL) {
			tail_->next = curit;
		}
		tail_ = curit;
		if(head_ == NULL) {
			head_ = curit;
		}
		length_++;		
	}

	QueueItem *MemList::Pop() 
	{
		if(head_ == NULL) {
			//reload data
			ContLoad();
		}
		if(head_ == NULL) return NULL;
		//File read over and set file status is fUse
		if(head_->data->Id() == 0) {
			if(filelist_->SetUse(head_->data->Fileid())) {	
				Delete();
			}
		}
		if(head_ == NULL) ContLoad();
		if(head_ == NULL) return NULL;
		return head_->data;
	}

	void MemList::Delete()
	{
		if(length_ == 0) return;
		if(head_ == NULL) return;
		QueueLink *curit = head_;
		head_ = head_->next;
		filelist_->SetItemNumber(curit->data->Fileid(), curit->data->Blockid(), curit->data->Offset(), curit->data->Id());
		currentMem_ -= curit->data->Size(); 
		if(currentMem_ < 0) currentMem_ = 0;
		delete curit;
		length_--;
	}

	/**
	 *data is read over from memory, continue load data from file
	 *if reader_ is not NULL, continue to use it, or load the
	 *other file
	 */
	void MemList::ContLoad()
	{
		FileId tmpCurrReadFid = currentReadFid_;
		FileId tmpCurrWriteFid = filelist_->GetCurrentFileId();
		int readOver = 0; 
		if(reader_) {
			readOver = LoadFile(currentReadFid_, 0, tmpCurrWriteFid);
		}
		if(readOver == 1) return;
		QueueItem *item = new QueueItem("", 0, 0, 0 , tmpCurrReadFid);
		Push(item);                                                   
		//Continue load other file
		FILELIST fileMapList;
		filelist_->GetUnUse(fileMapList);
		HoldLoad(fileMapList, filelist_->GetCurrentFileId());
	}

	uint64_t MemList::Load(FILELIST &flist, FileId curFileid)
	{
		Version::Instance()->Init(0,0);
		HoldLoad(flist, curFileid);
		//cout << "=============start SetCurrWriterPos "<< endl;
		SetCurrWriterPos(curFileid);
		return 0;	
	}

	void MemList::HoldLoad(FILELIST &flist, FileId curFileid)
	{
		FILELIST::iterator it;
		int readOver = 0;
		for(it = flist.begin(); it!=flist.end(); it++) {
			if(readOver != 0) break;
			size_t bid = it->second.blockid;
			if(bid==0) bid = 1;
			uint64_t pos = kBlockSize*(bid-1) + it->second.offset;
			currentReadFid_ = it->second.id;
			readOver = LoadFile(it->second.id, pos, curFileid);
			//File read over and set file status is fUse 	
			if(readOver == 0 && it->second.id != curFileid) {
				QueueItem *item = new QueueItem("", 0, 0, 0 , it->second.id);
				Push(item);	
			}
		}
	}


	Reader *MemList::GetCurrentReader(FileId fid, uint64_t pos) 
	{
		if(reader_) return reader_;
		string filename;
		QueueFileName::List(fid, filename);
		//filename = Opt::GetBasePath() + filename;
		Files f;
		SequentialFile* file;
		if(!f.NewSequentialFile(filename, &file)) {
			cout << "new read null "<< endl;
		  	return NULL;		
		}
		reader_ = new Reader(file,false,pos);
		return reader_;
	}

	int MemList::LoadFile(FileId fid, uint64_t p, FileId curFileid) 
	{
		Reader *r = GetCurrentReader(fid, p);
		string record;
		string scratch;
		uint32_t id;
		uint32_t blockid=0;
		while((id = r->ReadRecord(record, scratch, blockid))!=0){
			if(!id) continue;
			if(currentMem_ > mMaxBufferSize) return 1; 
			uint64_t blockOffset = r->BlockEndOffset();
			QueueItem *it = new QueueItem(record, id, blockid, blockOffset , fid);
			Push(it);
			currentMem_ += record.size();
		}
		if(curFileid == fid) return 1;
		delete r;
		reader_ = NULL;
		currentReadFid_ = 0;
		return 0;
	}

	void MemList::SetCurrWriterPos(FileId curFileid)
	{
		string filename;
		QueueFileName::List(curFileid, filename);
		//filename = Opt::GetBasePath() + filename;
		Files f;
		SequentialFile* file;
	  	if(!f.NewSequentialFile(filename, &file)) {
	  		cout << "MemList::SetCurrWriterPos failed" << endl;
			exit(1);
	  	}
		Reader reader(file,false,0);
		string record;
		string scratch;
		uint32_t id;
		uint32_t blockid;
		while((id = reader.ReadRecord(record, scratch, blockid))!=0){
			if(!id) continue;
			Version::Instance()->SetBlockId(blockid);
			Version::Instance()->SetBlockInterId();
		}
		delete file;
		uint64_t fileOffset = reader.FileEndOffset();
		uint64_t blockOffset = reader.BlockEndOffset();	
		//cout << "file offset: " << fileOffset  <<"\tblock offset: "<<blockOffset << endl;
		writer_->SetOffset(fileOffset, blockOffset);
	}



	void MemList::ReadTest()
	{
		QueueLink *qk;
		qk = head_;	
		int i=0;
		while(qk) {
			string str;
			cout << "id: "<< qk->data->Id() <<"\tlength: " << qk->data->Size() << "\tfileid: "<< qk->data->Fileid();
			qk->data->Str(str);
			cout <<str;
			cout << "\tblockid: "<<qk->data->Blockid()<<"\toffset: "<< qk->data->Offset();
			cout<<endl;
			qk = qk->next;
			//if(i==10)break;
			i++;
		}
	}

	uint64_t MemList::Size() 
	{
		return length_;
	}
}