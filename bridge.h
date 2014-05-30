#ifndef BRIDGE_H_
#define BRIDGE_H_

#include "include/queue.h"
#include "format.h"
#include "util/lock.h"

namespace pile {

 class Bridge : public Queue {
 		CommLock rLock_[levelNum];
		CommLock wLock_[levelNum];
	public:
		Bridge(){}
		~Bridge(){}
		bool Load(const string &path, const string &name);
		
		virtual bool Read(std::string &str);
		virtual bool Write(char *str, uint64_t length);
		virtual uint32_t Size();		
 };

}

	bool pile::Queue::Open(const string &path, const string &name, Queue **que) {
		Bridge *bge = new Bridge();
	 	if(!bge->Load(path, name)) {
			delete bge;
			return false;
		}
		*que = bge;
			
		return true;	

	}
#endif