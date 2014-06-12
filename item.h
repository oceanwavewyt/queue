#ifndef ITEM_H_
#define ITEM_H_

#include "include/queue.h"
#include "format.h"
namespace levelque {
 
 class MemList;
 class Item {
		std::string path_;
		std::string name_;	
		std::map<uint8_t, MemList *> mem_;
	public:	
		Item(const std::string &path, const std::string &name) {
			path_ = path;
			name_ = name;
		}
		~Item();
	public:
		std::string GetBasePath() {
			return  path_ + "/" + name_;
		}
		bool Initial();
		
		bool Read(std::string &str, uint8_t level=0);
		bool Write(char *str, uint64_t length, uint8_t level=0);
		uint32_t Size(uint8_t level=0);	
 };
}
#endif
