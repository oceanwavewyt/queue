#ifndef QUEUE_INCLUDE_H_ 
#define QUEUE_INCLUDE_H_ 

#include <string>
#include <sys/types.h>
#include <stdint.h>

class Queue
{
public:
	static bool Open(std::string &path, std::string &name, Queue **que);
	
	Queue(){
	}
	~Queue() {
	
	}
	virtual bool Read(std::string &str) = 0;
	virtual bool Write(char *str, uint64_t length)=0;
	virtual uint32_t Size() = 0;
};

#endif
