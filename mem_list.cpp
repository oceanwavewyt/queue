#include "mem_list.h"
#include "file_env.h"
#include "reader.h"
#include "writer.h"
#include "version.h"

MemList *MemList::instance_ = NULL;

MemList *MemList::Instance()
{
	if(instance_) return instance_;
	instance_ = new MemList();
	return instance_;
}

MemList::MemList():head_(NULL),tail_(NULL),currentMem_(0) {
	length_ = 0;
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

void MemList::WriteRecord(const string &str, size_t length) {
	//cout << "blockid: "<< Version::Instance()->GetBlockId() << endl;	
	if(Version::Instance()->GetBlockId() >= fMaxBlockNum) {
		filelist_->ReleaseCurFile();
		string filename;
		filelist_->GetCurrentFile(filename);
		SetWriter(filename);	
	}
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
		return NULL;	
	}
	return head_->data;
}

void MemList::Delete()
{
	if(length_ == 0) return;
	if(head_ == NULL) return;
	QueueLink *curit = head_;
	head_ = head_->next;
	filelist_->SetItemNumber(curit->data->Fileid(), curit->data->Blockid(), curit->data->Offset(), curit->data->Id());
	delete curit;
	length_--;
}

uint64_t MemList::Load(FILELIST &flist, FileId curFileid)
{
	FILELIST::iterator it;
	int readOver = 0;
	for(it = flist.begin(); it!=flist.end(); it++) {
		Version::Instance()->Init(0,0);
		if(readOver == 0) {
			size_t bid = it->second.blockid;
			if(bid==0) {
				bid = 1;
			}
			uint64_t pos = kBlockSize*(bid-1) + it->second.offset;
			//cout <<"readOver: "<<readOver <<" blockid: "<<it->second.blockid<< "  block_offset: "<< it->second.offset << endl;
			readOver = LoadFile(it->second.id, pos);
		}
	}
	//cout << "=============start SetCurrWriterPos "<< endl;
	SetCurrWriterPos(curFileid);
	return 0;	
}

Reader *MemList::GetCurrentReader(FileId fid, uint64_t pos) 
{
	if(reader_) return reader_;
	string filename;
	QueueFileName::List(fid, filename);
	Files f;
	SequentialFile* file;
	if(!f.NewSequentialFile(filename, &file)) {
	  	return NULL;		
	}
	reader_ = new Reader(file,false,pos);
	return reader_;
}

int MemList::LoadFile(FileId fid, uint64_t p) 
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
	delete r;
	reader_ = NULL;
	return 0;
}

void MemList::SetCurrWriterPos(FileId curFileid)
{
	string filename;
	QueueFileName::List(curFileid, filename);
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
	while(qk) {
		cout << "id: "<< qk->data->Id() <<"\tlength: " << qk->data->Size() << "\tfileid: "<< qk->data->Fileid();
		//qk->data->Str();
		cout << "\tblockid: "<<qk->data->Blockid()<<"\toffset: "<< qk->data->Offset();
		cout<<endl;
		qk = qk->next;
	}
}

uint64_t MemList::Size() 
{
	return length_;
}
