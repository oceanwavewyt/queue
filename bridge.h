#ifndef BRIDGE_H_
#define BRIDGE_H_

#include "include/queue.h"
#include "format.h"

class Bridge : public Queue
{
public:
	Bridge(){}
	~Bridge(){}
	bool Load(const string &path, const string &name);
	
	virtual bool Read(std::string &str);
	virtual bool Write(char *str, uint64_t length);
	virtual uint32_t Size();	
	
};


bool Queue::Open(const string &path, const string &name, Queue **que) {
	Bridge *bge = new Bridge();
 	if(!bge->Load(path, name)) {
		delete bge;
		return false;
	}
	*que = bge;
		
	return true;	

}

#endif
