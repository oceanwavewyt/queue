#ifndef QUEUE_INCLUDE_H_ 
#define QUEUE_INCLUDE_H_ 

#include <string>
#include <sys/types.h>
#include <stdint.h>

namespace levelque {
	class Queue
	{
	public:
		static bool Open(const std::string &path, const std::string &name, Queue **que);
		
		Queue(){
		}
		~Queue() {
		
		}
		virtual bool CreateQueueName(const std::string &path, const std::string &name) = 0;
		virtual bool Read(const std::string &queueName, std::string &str, uint8_t level) = 0;
		virtual bool Read(const std::string &queueName, std::string &str) = 0;
		virtual bool Write(const std::string &queueName, const char *str, uint64_t length, uint8_t level)=0;
		virtual uint32_t Size(const std::string &queueName, uint8_t level) = 0;
		virtual uint32_t Size(const std::string &queueName) = 0;
	};
}
#endif
