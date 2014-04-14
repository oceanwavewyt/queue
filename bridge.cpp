#include "bridge.h"
#include "format.h"
#include "file_env.h"
#include "reader.h"
#include "writer.h"
#include "mem_list.h"
/*
Bridge::Bridge() 
{

}

Bridge::~Bridge()
{

}
*/
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
	Files f;
	/*********************/
	FixFile *fFile;
	string filename;
	QueueFileName::Head(filename);
	//string readPath = Opt::GetBasePath() + filename;
	f.NewFixFile(filename, &fFile);
	if(fFile->LoadFile() == false) {
		cout << "filelist load failed." << endl;
		exit(1);
	}
	//load to memory
	FILELIST fileMapList;
	fFile->GetUnUse(fileMapList);

	fFile->GetCurrentFile(filename);
	
	MemList::Instance()->SetFilelist(fFile);
	MemList::Instance()->SetWriter(filename);
	if(fileMapList.size() > 0) {
		FileId ccid = fFile->GetCurrentFileId();
		MemList::Instance()->Load(fileMapList,ccid);
	}
	
	return true;
}

bool Bridge::Read(std::string &str) 
{
	QueueItem *item = MemList::Instance()->Pop();
	if(item) {
		item->Str(str);
		MemList::Instance()->Delete();
		return true;	
	}	
	return false;
}

bool Bridge::Write(char *str, uint64_t length)
{
	MemList::Instance()->WriteRecord(str, length);
	return true;
}

uint32_t Bridge::Size()
{
	return 1;
}
