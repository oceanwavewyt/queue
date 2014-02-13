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
	loadinfo_.isComplete = true;
	loadinfo_.fid = 0;
	loadinfo_.pos = 0;
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
	cout << "blockid: "<< Version::Instance()->GetBlockId() << endl;	
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
	delete curit;
	length_--;
}

uint64_t MemList::Load(FILELIST &flist, FileId curFileid)
{
	FILELIST::iterator it;
	uint64_t num = 0;
	for(it = flist.begin(); it!=flist.end(); it++) {
		//cout <<"time: "<< it->first << "\tid: " << it->second << endl;
		Version::Instance()->Init(1,1);
		num += LoadFile(it->second, curFileid);
		
	}
	return num;	
}

uint64_t MemList::LoadFile(FileId fid, FileId curFileid) 
{
	string filename;
	uint64_t num=0;
	QueueFileName::List(fid, filename);
	Files f;
	SequentialFile* file;
  	if(!f.NewSequentialFile(filename, &file)) {
  		return num;
  	}
	Reader reader(file,false,0);
	string record;
	string scratch;
	uint32_t id;
	while((id = reader.ReadRecord(record, scratch))!=0){
		if(!id) continue;
		if(currentMem_ <= mMaxBufferSize) {
			QueueItem *it = new QueueItem(record, id, fid);
			Push(it);
			currentMem_ += record.size();
			loadinfo_.fid = fid;
			loadinfo_.pos = id;
			//cout << "currentMem_: "<< currentMem_/1024/1024 << endl;
		}else{
			loadinfo_.isComplete = false;
			if(curFileid != fid) break;
		}
		num++;
	}
	if(curFileid == fid) {
		uint64_t fileOffset = reader.FileEndOffset();
		uint64_t blockOffset = reader.BlockEndOffset();	
		cout << "file offset: " << fileOffset  <<"\tblock offset: "<<blockOffset << endl;
		writer_->SetOffset(fileOffset, blockOffset);
	}

	delete file;	
	return num;
}

void MemList::ReadTest()
{
	QueueLink *qk;
	qk = head_;	
	while(qk) {
		cout << "id: "<< qk->data->Id() <<"\tlength: " << qk->data->Size() << "\tfileid: "<< qk->data->Fileid();
		//qk->data->Str();
		cout<<endl;
		qk = qk->next;
	}
}

uint64_t MemList::Size() 
{
	return length_;
}
