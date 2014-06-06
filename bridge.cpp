#include "bridge.h"
#include "format.h"
#include "file_env.h"
#include "reader.h"
#include "writer.h"
#include "mem_list.h"

namespace levelque {
	std::string Opt::path_ = "./data";
	std::string Opt::name_ = "test";

bool Bridge::Load(const string &path, const string &name) 
	{
	if(opendir(path.c_str()) == NULL) {
		mkdir(path.c_str(), 0755);	
	}
	string baseName = path + "/" + name;
	if(opendir(baseName.c_str()) == NULL) {
		mkdir(baseName.c_str(), 0755);	
	}
	Opt::Set(path, name);
	string levelFilename;
	QueueFileName::Level(levelFilename); 
	
	
	for(uint8_t i=0; i<=levelNum; i++) {
		MemList::Instance(i)->LoadAll();
	}
	
	return true;
}

	bool Bridge::Read(std::string &str, uint8_t level) 
	{
		QueueItem *item = MemList::Instance(level)->Pop();
		if(item) {
			item->Str(str);
			MemList::Instance(level)->Delete();
			return true;	
		}	
		return false;
	}

	bool Bridge::Write(char *str, uint64_t length, uint8_t level)
	{
		MemList::Instance(level)->WriteRecord(str, length);
		return true;
	}

	uint32_t Bridge::Size(uint8_t level)
	{
		return 1;
	}
}
