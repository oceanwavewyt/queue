#ifndef MEM_LIST_H_
#define MEM_LIST_H_
#include "format.h"
#include "file_env.h"

namespace levelque {

 class Item;
 class Version;	
 class Writer;
 class Reader;
 class QueueItem {
		uint32_t length_;
		uint32_t id_;
		uint32_t bid_;
		uint64_t blockOffset_;
		FileId fid_;
		string str_;
	public:
		QueueItem(const string &record, uint32_t id, uint32_t bid, uint64_t blockOffset , FileId fid):
			id_(id),bid_(bid),blockOffset_(blockOffset), fid_(fid) {
			//assert(record.size()!=0);
			length_ = record.size();
			//str_ = (char *)malloc(length_+1);
			//memset(str_,0,length_);
			//memcpy(str_, record.data(), length_);
			str_.assign(record.data(), length_);
		}
		uint32_t Size() {
			return length_;
		}
		uint32_t Id() {
			return id_;
		}
		uint32_t Blockid() {
			return bid_;
		}
		uint64_t Offset() {
			return blockOffset_;
		}
		FileId Fileid() {
			return fid_;
		}
		void Str(string &str) {
			//cout << str_ << endl;
			str.assign(str_.data(), length_);
		}
		~QueueItem() {
			//if(str_) free(str_);
		}	
 };

	class QueueLink {
	public:
		QueueLink(QueueItem *it){ 
			data = it;
			next = NULL;
		}
		~QueueLink(){
			delete data;
		}
	public:	
		QueueItem *data;
		QueueLink *next;
	};

	class MemList
	{
		static MemList *instance_[levelNum];
		FixFile *filelist_;
	public:
		static MemList *Instance(const uint8_t levelid=0);
		static void Initize();
		MemList(Item *,uint8_t levelid=0);	
		~MemList();
		uint64_t Load(FILELIST &list, FileId curFileid);
		void Push(QueueItem *item);	
		QueueItem *Pop();
		void Delete();
		uint64_t Size();
		void LoadAll();
		
		void SetWriter(string &filename); 
		void WriteRecord(const string &str, size_t length, uint8_t level=0);

		void ReadTest(); 
	private:
		int LoadFile(FileId id, uint64_t pos, FileId curFile);
		void HoldLoad(FILELIST &list, FileId curFileid);
		void ContLoad();
		Reader *GetCurrentReader(FileId fid, uint64_t pos=0);
		void SetCurrWriterPos(FileId curFileid); 
	private:
		Item *item_;
		QueueLink *head_;
		QueueLink *tail_;
		uint64_t length_;
		Writer *writer_;
		Reader *reader_;
		uint64_t currentMem_;
		FileId currentReadFid_;
		uint8_t level_;
		Version *ver_;
	};
}
#endif
