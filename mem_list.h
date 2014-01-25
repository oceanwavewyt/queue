
class QueueItem {
	uint32_t length_;
	uint32_t id_;
	char *str_;
public:
	QueueItem(char *str, uint32_t length, uint32_t id) {
		
	}	
};

typedef struct queueLink {
		
	QUEUEITEM *next;
}QUEUELINK;

class MemList
{
	static memList *instance_;
public:
	static MemList *Instance();
	MemList();	
	~MemList();
	void Push(QUEUEITEM *item);	
	QUEUEITEM *Pop();	
};
