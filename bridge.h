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
		delete bge;
		return false;
	}
	*que = bge;
		
	return true;	

}
