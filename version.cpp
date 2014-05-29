#include "version.h"

namespace pile {
Version *Version::instance_ = NULL;

Version::Version():blockid_(1),interid_(1){

}
Version::~Version(){

}
void Version::Init(uint32_t blockid, uint32_t interid)
{
	blockid_ = blockid;
	interid_ = interid;
}

Version* Version::Instance()
{
	if(instance_) return instance_;
	instance_ = new Version();
	return instance_;
}


void Version::SetBlockId(uint32_t id)
{
	if(blockid_ != id) {
		interid_ = 1;
	}
	blockid_ = id;

}

void Version::SetBlockId()
{
	blockid_++;
	interid_ = 1;
}

void Version::SetBlockInterId()
{
	interid_++;	
}

uint32_t Version::GetBlockId()
{
	return blockid_;
}
uint32_t Version::GetInterId()
{
	interid_++;
	return interid_;
}
}