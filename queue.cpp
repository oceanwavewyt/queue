#include "format.h"
#include "file_env.h"
#include "reader.h"
#include "writer.h"
#include "mem_list.h"

#include <iostream>
#include <fstream>

using namespace std;



size_t readtest(string &filename, string &r) {
	char * buffer;
    size_t size;
	 ifstream file(filename.c_str(), ios::in|ios::binary|ios::ate);
	 size = file.tellg();
	 file.seekg (0, ios::beg);
	 buffer = new char [size];
	 file.read (buffer, size);
	 r.assign(buffer, size);
	 delete []buffer;
	 file.close();
	 return size;
}

void TestWrite() {
	/*
	MmapFile *mfile;

	f.NewWritableFile(filename, &mfile);	
	Writer* w = new Writer(mfile);
	*/
	string testData;
	testData = "我是ddfdf你大爷qqqqq";
	MemList::Instance()->WriteRecord(testData, testData.size());

	char testData2[10] = "123456789";	
	MemList::Instance()->WriteRecord(testData2, 10);
	/*
	string dataFilename = "/root/wm_8d580aa8adddaa3776a8cd1516d832ab.jpg";
	string imgdata;
	size_t len = readtest(dataFilename, imgdata);
	w->AddRecord(imgdata, len);
	*/
	/*
	ofstream out2("../tmp/444.jpg",ios::binary);
		out2<<imgdata;
		out2.close();
	*/
}

void TestRead(Files &f, string &filename) {
	SequentialFile* file;
  	f.NewSequentialFile(filename, &file);
	Reader reader(file,false,0);
	string record;
	string scratch;
	int i=0;
	uint32_t id;
	while((id = reader.ReadRecord(record, scratch))!=0){
		//cout << "read: " << record << endl;
		//if(i==2) {
		//char name[20];
		//sprintf(name, "../tmp/%d.jpg",i);
		//ofstream out(name,ios::binary);
		//out<<record;
		//out.close();
		//}else{
			cout << "read: " << record << endl;
		//}
		cout <<"id: "<< id << endl;
		i++;
	}
}


int main(int argc, char *args[]) 
{
	Files f;
	
	/*********************/
	FixFile *fFile;
	f.NewFixFile("./data/filelist.dat", &fFile);
	//
	if(fFile->LoadFile() == false) {
		cout << "filelist load failed." << endl;
		exit(1);
	}
	//load to memory
	std::map<TimeId, FileId> fileMapList;
	fFile->GetUnUse(fileMapList);

	string filename;
	fFile->GetCurrentFile(filename);
	cout << "current file: " << filename << endl;

	MemList::Instance()->SetWriter(filename);

	MemList::Instance()->Load(fileMapList,fFile->GetCurrentFileId());
	
	MemList::Instance()->ReadTest();	

	

	
	/*********************/
	TestWrite();	
	
	//TestRead(f, filename);	
	//MemList::Instance()->Load(fileMapList,fFile->GetCurrentFileId());
	
	//MemList::Instance()->ReadTest();	
	return 1;
}
