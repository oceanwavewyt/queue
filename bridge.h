#ifndef BRIDGE_H_
#define BRIDGE_H_

#include "include/queue.h"
#include "format.h"
#include "util/lock.h"

namespace levelque {
 
 class Item;
 class Bridge : public Queue {
 		CommLock rLock_[levelNum];
		CommLock wLock_[levelNum];
		std::map<string, Item *> ques_;
	public:
		Bridge(){}
		~Bridge(){}
		virtual bool CreateQueueName(const string &path, const string &name);
		virtual bool Read(const std::string &queueName, std::string &str, uint8_t level);
		virtual bool Write(const std::string &queueName, char *str, uint64_t length, uint8_t level);
		virtual uint32_t Size(const std::string &queueName, uint8_t level);
		virtual uint32_t Size(const std::string &queueName);		
 };

}

	bool levelque::Queue::Open(const string &path, const string &name, Queue **que) {
		Bridge *bge = new Bridge();
	 	if(!bge->CreateQueueName(path, name)) {
			delete bge;
			return false;
		}
		*que = bge;
			
		return true;	

	}
#endif
