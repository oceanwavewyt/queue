#ifndef WRITER_H_
#define WRITER_H_

#include "format.h"
class MmapFile;

class Writer 
{
	MmapFile *dest_;
	int block_offset_;
public:
	Writer(MmapFile *dest);
	~Writer();
	bool AddRecord(const string &data, size_t length);
	void SetOffset(int offset);
private:
	bool EmitPhysicalRecord(RecordType t, const char* ptr, size_t n);                                                       
};

#endif