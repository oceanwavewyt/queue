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
		for(int j=0;j<140;j++) {	
			char testData2[100]={0};
			sprintf(testData2, "%d_共产主义好，生活幸福美满",j);	
			//sprintf(testData2, "%d_abcde,efghijk",j);
			//levelque::MemList::Instance()->WriteRecord(testData2, strlen(testData2));
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

	void TestRead(levelque::Files &f, const string &filename) {
		levelque::SequentialFile* file;
	  	f.NewSequentialFile(filename, &file);
		levelque::Reader reader(file,false,0);
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
		levelque::Queue *db;
		levelque::Queue::Open("/tmp","abc", &db);
	
		db->CreateQueueName("/tmp", "abc_two");
		
		string data;
		char testData2[100]={0};                            
		/*
		sprintf(testData2, "%ld_共产主义好，生活幸福美满_1",time(NULL));
		db->Write("abc",testData2, strlen(testData2),1);
		
		for(int i=1;i<=10; i++) {
			sprintf(testData2, "%ld_%d_共产主义好，生活幸福美满_2",time(NULL),i);
			db->Write("abc", testData2, strlen(testData2),2);
			cout << "abc number: "<< db->Size("abc") << endl;
		}
		*/
		if(db->Read("abc", data)) {
			cout << "max read: " << data << endl;
		}
		
		return 1;

		sprintf(testData2, "%ld_test_dbtwo_2",time(NULL));
		db->Write("abc_two", testData2, strlen(testData2),2);
		cout << "abc_two number: "<< db->Size("abc_two") << endl;	
		
		if(db->Read("abc_two", data, 2)) {
			cout << "read: " << data << endl;
		}
	
				

		return 1;
	}
