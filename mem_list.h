#include "format.h"

class QueueItem {
	uint32_t length_;
	uint32_t id_;
	char *str_;
public:
	QueueItem(char *str, uint32_t length, uint32_t id) {
		
	}	
};

typedef struct queueLink {
	QueueItem *data;
	struct queueLink *next;
} QUEUELINK;

class MemList
{
	static MemList *instance_;
public:
	static MemList *Instance();
	MemList();	
	~MemList();
	void Push(QueueItem *item);	
	QueueItem *Pop();	
};
