#include "mem_list.h"

MemList *MemList::instance_ = NULL;

MemList *MemList::Instance()
{
	if(instance_) return instance_;
	instance_ = new MemList();
	return instance_;
}

MemList::MemList(){

}

MemList::~MemList(){

}

void MemList::Push(QueueItem *item) 
{
	
}


