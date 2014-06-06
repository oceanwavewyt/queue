#ifndef QUEUE_INCLUDE_H_ 
#define QUEUE_INCLUDE_H_ 

#include <string>
#include <sys/types.h>
#include <stdint.h>

namespace pile {
	class Queue
	{
	public:
		static bool Open(const std::string &path, const std::string &name, Queue **que);
		
		Queue(){
		}
		~Queue() {
		
		}
		virtual bool Read(std::string &str, uint8_t level=0) = 0;
		virtual bool Write(char *str, uint64_t length, uint8_t level=0)=0;
		virtual uint32_t Size(uint8_t level=0) = 0;
	};
}
#endif
