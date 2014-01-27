#include "queue.h"

class Bridge : public Queue
{
public:
	Bridge();
	~Bridge();
	bool Load();
	
};


bool Queue::Open(string &path, string &name, Queue **que) {
	Bridge *bge = new Bridge();
 	if(!bge->Load()) {
		return false;
	}
	*que = bge;
		
	return true;	

}
