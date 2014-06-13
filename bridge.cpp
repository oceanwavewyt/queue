#include "bridge.h"
#include "format.h"
#include "file_env.h"
#include "reader.h"
#include "writer.h"
#include "mem_list.h"
#include "item.h"

namespace levelque {

	bool Bridge::CreateQueueName(const string &path, const string &name)
	{
		if(ques_.find(name) != ques_.end()) {
			return true;
		}	
		ques_[name] = new Item(path, name);		
		ques_[name]->Initial();		
		return true;
	}

	bool Bridge::Read(const std::string &queueName, std::string &str, uint8_t level) 
	{
		if(ques_.find(queueName) == ques_.end()) {
			return false;
		}
		return ques_[queueName]->Read(str, level);
	}

	bool Bridge::Write(const std::string &queueName, char *str, uint64_t length, uint8_t level)
	{
		if(ques_.find(queueName) == ques_.end()) {
			return false;
		}	
		return ques_[queueName]->Write(str, length, level);
	}

	uint32_t Bridge::Size(const std::string &queueName, uint8_t level)
	{
		if(ques_.find(queueName) == ques_.end()) {
			return 0;
		}
		return ques_[queueName]->Size(level);
	}

	uint32_t Bridge::Size(const std::string &queueName)
	{
		if(ques_.find(queueName) == ques_.end()) {
			return 0;
		}
		return ques_[queueName]->SizeAll();
	}
}
