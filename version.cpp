#include "version.h"
#include "file_env.h"

namespace levelque {
Version *Version::instance_ = NULL;

Version::Version():blockid_(1),interid_(1),filelist_(NULL){

}
Version::~Version(){

}

void Version::SetFilelist(FixFile *f)
{
	filelist_ = f;
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
		if(filelist_) {
			filelist_->SetWriteInterid(interid_);
		}
	}
	blockid_ = id;
	if(filelist_) {
		filelist_->SetWriteBlock(blockid_);
	}
}

void Version::SetBlockId()
{
	blockid_++;
	cout << "SetBlockId: "<< blockid_ << endl;
	interid_ = 1;
	//if(filelist_) {
		
		filelist_->SetWriteBlock(blockid_);
		filelist_->SetWriteInterid(interid_);
	//}
}

void Version::SetBlockInterId()
{
	interid_++;
	if(filelist_) {
		filelist_->SetWriteInterid(interid_);
	}                                      	
}

uint32_t Version::GetBlockId()
{
	return blockid_;
}
uint32_t Version::GetInterId()
{
	interid_++;
	if(filelist_) {
		filelist_->SetWriteInterid(interid_);
	}
	return interid_;
}

void Version::SetBlockOffset(uint64_t offset)
{
	filelist_->SetWriteOffset(offset);
}

}
