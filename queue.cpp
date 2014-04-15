#include "format.h"
#include "file_env.h"
#include "reader.h"
#include "writer.h"
#include "mem_list.h"
#include "include/queue.h"
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
	/*	
	string testData;
	testData = "我是ddfdf你大爷qqqqq";
	cout << "write length:" << testData.size() <<endl;
	MemList::Instance()->WriteRecord(testData, testData.size());
	*/
	for(int j=0;j<140;j++) {	
		char testData2[100]={0};
		sprintf(testData2, "%d_共产主义好，生活幸福美满",j);	
		//sprintf(testData2, "%d_abcde,efghijk",j);
		MemList::Instance()->WriteRecord(testData2, strlen(testData2));
	}
	/*
	char testData3[15] = "33333333333333";	
	MemList::Instance()->WriteRecord(testData3, 15);
		
	string dataFilename = "1.jpg";
	string imgdata;
	size_t len = readtest(dataFilename, imgdata);
	MemList::Instance()->WriteRecord(imgdata, len);
	
	
	ofstream out2("../tmp/444.jpg",ios::binary);
		out2<<imgdata;
		out2.close();
	*/
}

void TestRead(Files &f, const string &filename) {
	SequentialFile* file;
  	f.NewSequentialFile(filename, &file);
	Reader reader(file,false,0);
	string record;
	string scratch;
	int i=0;
	uint32_t id;
	uint32_t blockid;
	while((id = reader.ReadRecord(record, scratch, blockid))!=0){
		//cout << "read: " << record << endl;
		//if(i==2) {
		//char name[20];
		//sprintf(name, "../tmp/%d.jpg",i);
		//ofstream out(name,ios::binary);
		//out<<record;
		//out.close();
		//}else{
			//cout << "read: " << record << endl;
		//}
		//cout <<"id: "<< id << endl;
		//cout << record << " read offset: "<< reader.LastRecordEndOffset() << endl;
		i++;
	}
}

int main(int argc, char *args[])
{
	Queue *db;
	Queue::Open("/root","abc", &db);
	
	char testData2[100]={0};                            
	sprintf(testData2, "%d_共产主义好，生活幸福美满",133);
	db->Write(testData2, strlen(testData2));
	
	string data;
	if(db->Read(data)) {
		cout << "read: " << data << endl;
	}
	
	return 1;
}

int main23333333(int argc, char *args[]) 
{
	Files f;
	
	//TestRead(f, "./data/queue_0001.lst");
	//return 1;
	/*********************/
	FixFile *fFile;
	f.NewFixFile("./data/filelist.dat", &fFile);
	//
	if(fFile->LoadFile() == false) {
		cout << "filelist load failed." << endl;
		exit(1);
	}
	//load to memory
	FILELIST fileMapList;
	fFile->GetUnUse(fileMapList);

	string filename;
	fFile->GetCurrentFile(filename);
	//cout << "current file: " << filename << endl;
	
	MemList::Instance()->SetFilelist(fFile);
	MemList::Instance()->SetWriter(filename);


	MemList::Instance()->Load(fileMapList,fFile->GetCurrentFileId());
	
	MemList::Instance()->ReadTest();
	//read data
	if(argc <2) return 1;
	
	if(strcmp(args[1],"read")==0) {
		int i = 0;
		while(i<10) {
			cout << "pop data" << endl;
			QueueItem *item = MemList::Instance()->Pop();	
			if(item) {	
				cout << "pop: "<< item->Id() << "\tlength:"<< item->Size() <<"\t";	
				string cont;
				item->Str(cont);
				cout << cont << endl;
				/**
				ofstream file("testout/write.jpg", ios::out|ios::binary|ios::ate);
				file << cont;
				*/
				MemList::Instance()->Delete();
			}
			i++;
		}
	}
	/*********************/
	if(strcmp(args[1], "write")==0) {
		//cout << "start write "<< endl;
		TestWrite();
	}	
	//TestRead(f, filename);	
	//MemList::Instance()->Load(fileMapList,fFile->GetCurrentFileId());
	
	//MemList::Instance()->ReadTest();	
	return 1;
}
