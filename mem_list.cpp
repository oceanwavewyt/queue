#include "mem_list.h"
#include "file_env.h"
#include "reader.h"

MemList *MemList::instance_ = NULL;

MemList *MemList::Instance()
{
	if(instance_) return instance_;
	instance_ = new MemList();
	return instance_;
}

MemList::MemList():head_(NULL),tail_(NULL){
	length_ = 0;
}

MemList::~MemList(){

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

uint64_t MemList::Load(FILELIST &flist)
{
	FILELIST::iterator it;
	uint64_t num = 0;
	for(it = flist.begin(); it!=flist.end(); it++) {
		cout << it->first << "\t"<< (FileId)it->second <<endl;
		num += LoadFile(it->second);	
	}
	return num;	
}

uint64_t MemList::LoadFile(FileId fid) 
{
	string filename;
	QueueFileName::List(fid, filename);
	Files f;
	SequentialFile* file;
  	f.NewSequentialFile(filename, &file);
	Reader reader(file,false,0);
	string record;
	string scratch;
	uint64_t num=0;
	uint32_t id;
	while((id = reader.ReadRecord(record, scratch))!=0){
		//cout <<"id: "<< id << endl;
		if(!id) continue;
		cout << "fid: "<< fid << endl;
		QueueItem *it = new QueueItem(record, id, fid);
		Push(it);
		num++;
	}	
	delete file;	
	return num;
}

void MemList::ReadTest()
{
	QueueLink *qk;
	qk = head_;	
	while(qk) {
		cout << "id: "<< qk->data->Id() <<"\tlength: " << qk->data->Size() << "\tfileid: "<< qk->data->Fileid() <<endl;
		qk = qk->next;
	}
}

uint64_t MemList::Size() 
{
	return length_;
}
