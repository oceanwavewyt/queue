#include "mem_list.h"

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
		num += LoadFile(it->second);	
	}
	return num;	
}

uint64_t MemList::LoadFile(FileId id) 
{
	string filename;
	QueueFileName::List(id, filename);
	
	return 1;
}

uint64_t MemList::Size() 
{
	return length_;
}
