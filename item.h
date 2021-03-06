#ifndef ITEM_H_
#define ITEM_H_

#include "include/queue.h"
#include "format.h"
#include "util/lock.h"

namespace levelque {
 class LevelFile; 
 class MemList;
 class Item {
		CommLock rLock_[levelNum];
		CommLock wLock_[levelNum];
		CommLock rMaxLock_;

		std::string path_;
		std::string name_;	
		std::map<uint8_t, MemList *> mem_;
		LevelFile *levelfile_;
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
		
		bool Read(std::string &str, uint8_t level);
		bool Read(std::string &str);
		bool Write(const char *str, uint64_t length, uint8_t level=0);
		uint32_t Size(uint8_t level=0);	
		uint32_t SizeAll();	
	private:
		uint8_t GetCurMaxLevel();  
  };
}
#endif
