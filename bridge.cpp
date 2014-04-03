#include "bridge.h"
#include "format.h"
/*
Bridge::Bridge() 
{

}

Bridge::~Bridge()
{

}
*/
bool Bridge::Load(string &path, string &name) 
{
	if(opendir(path.c_str()) == NULL) {
		mkdir(path.c_str(), 0755);	
	}	
	return true;
}

bool Bridge::Read(std::string &str) 
{

	return true;
}

bool Bridge::Write(char *str, uint64_t length)
{

	return true;
}

uint32_t Bridge::Size()
{
	return 1;
}
