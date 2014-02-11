#ifndef MEM_LIST_H_
#define MEM_LIST_H_
#include "format.h"
#include "file_env.h"

class Writer;
class QueueItem {
	uint32_t length_;
	uint32_t id_;
	FileId fid_;
	char *str_;
public:
	QueueItem(string &record, uint32_t id, FileId fid):id_(id),fid_(fid) {
		assert(record.size()!=0);
		length_ = record.size();
		str_ = (char *)malloc(record.size());
		memcpy(str_, record.data(), record.size());
	}
	uint32_t Size() {
		return length_;
	}
	uint32_t Id() {
		return id_;
	}
	FileId Fileid() {
		return fid_;
	}
	void Str() {
		cout << str_ << endl;
	}
	~QueueItem() {
		if(str_) free(str_);
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
	static MemList *instance_;
	FixFile *filelist_;
public:
	static MemList *Instance();
	MemList();	
	~MemList();
	uint64_t Load(FILELIST &list, FileId curFileid);
	void Push(QueueItem *item);	
	QueueItem *Pop();
	void Delete();
	uint64_t Size();
	
	void SetWriter(string &filename); 
	void WriteRecord(const string &str, size_t length);
	void SetFilelist(FixFile *f);

	void ReadTest(); 
private:
	uint64_t LoadFile(FileId id,FileId curFileid);
private:
	QueueLink *head_;
	QueueLink *tail_;
	uint64_t length_;
	Writer *writer_;	
};
#endif
