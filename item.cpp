#include "item.h"
#include "mem_list.h"

namespace levelque {

	Item::~Item() {
		for(uint8_t i=0; i<=levelNum; i++) {
			if(!mem_[i]) continue;
			delete mem_[i];
		}
	}
	bool Item::Initial() 
	{
		if(opendir(path_.c_str()) == NULL) {
			mkdir(path_.c_str(), 0755);	
		}
		string baseName = path_ + "/" + name_;
		if(opendir(baseName.c_str()) == NULL) {
			mkdir(baseName.c_str(), 0755);	
		}
		string levelFilename;
		QueueFileName::Level(levelFilename); 
			
		for(uint8_t i=0; i<=levelNum; i++) {
			mem_[i] = new MemList(this, i);
			mem_[i]->LoadAll();
		}
		return true;
	}

	bool Item::Read(std::string &str, uint8_t level)
	{
		if(level > levelNum) return false;	
		QueueItem *item = mem_[level]->Pop();
		if(item) {
			item->Str(str);
			mem_[level]->Delete();
			return true;	
		}	
		return false;	
	}
	
	bool Item::Write(char *str, uint64_t length, uint8_t level)
	{
		if(level > levelNum) return false;
		mem_[level]->WriteRecord(str, length);
		return true;
	}

	uint32_t Item::Size(uint8_t level)
	{
		if(level > levelNum) return 0;
		return 1;
	}
}
