#include "format.h"

class QueueItem {
	uint32_t length_;
	uint32_t id_;
	char *str_;
public:
	QueueItem(char *str, uint32_t length, uint32_t id):length_(length),id_(id) {
		assert(length!=0);
		str_ = (char *)malloc(length);
		memcpy(str_, str, length);		
	}
	~QueueItem() {
		if(str_) free(str_);
	}	
};

class QueueLink {
public:
	QueueLink(QueueItem *it){ 
		data = it;
	}
	~QueueLink(){
		delete data;
	}
public:	
	QueueItem *data;
	QueueLink *next;
};

class MemList
{
	static MemList *instance_;
public:
	static MemList *Instance();
	MemList();	
	~MemList();
	uint64_t Load(FILELIST &list);
	uint64_t LoadFile(FileId id); 
	void Push(QueueItem *item);	
	QueueItem *Pop();
	void Delete();
	uint64_t Size();
private:
	QueueLink *head_;
	QueueLink *tail_;
	uint64_t length_;	
};
