#include "item.h"
#include "mem_list.h"
#include "file_env.h"

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
		//load num file
		string levelFilename;
		QueueFileName::Level(levelFilename); 
		levelFilename = GetBasePath() + "/" + levelFilename;			
		Files f;	
		assert(f.NewLevelFile(levelFilename, &levelfile_));
		assert(levelfile_->LoadFile());		
		
		for(uint8_t i=0; i<=levelNum; i++) {
			mem_[i] = new MemList(this, i);
			mem_[i]->LoadAll();
		}
		return true;
	}

	bool Item::Read(std::string &str, uint8_t level)
	{
		if(level > levelNum) return false;	
		Lock lock(&rLock_[level]);
		QueueItem *item = mem_[level]->Pop();
		if(item) {
			item->Str(str);
			mem_[level]->Delete();
			levelfile_->SubItemNumber(level);
			return true;	
		}	
		return false;	
	}

	bool Item::Read(std::string &str)
	{
		Lock lock(&rMaxLock_);
		return Read(str, levelfile_->GetMaxLevel());
	}
	
	bool Item::Write(char *str, uint64_t length, uint8_t level)
	{
		Lock lock(&wLock_[level]);
		if(level > levelNum) return false;
		mem_[level]->WriteRecord(str, length);
		levelfile_->AddItemNumber(level);
		return true;
	}

	uint32_t Item::Size(uint8_t level)
	{
		if(level > levelNum) return 0;
		return levelfile_->GetNumber(level);
	}

	uint32_t Item::SizeAll()
	{
		return levelfile_->GetNumber();
	}

	//get the current's max level  for reading
	uint8_t Item::GetCurMaxLevel()
	{
		return levelfile_->GetMaxLevel();	
	}
}
