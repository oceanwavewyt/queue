#ifndef WRITER_H_
#define WRITER_H_

#include "format.h"
namespace levelque {
	class MmapFile;
	class Version;	

	class Writer 
	{
		MmapFile *dest_;
		int block_offset_;
		uint32_t type_crc_[kMaxRecordType + 1];
		Version *ver_;
	public:
		Writer(MmapFile *dest);
		~Writer();
		bool AddRecord(const string &data, size_t length);
		void SetOffset(int fileOffset, int blockOffset);
		void SetVersion(Version *ver);
	private:
		bool EmitPhysicalRecord(RecordType t, const char* ptr, size_t n);                                                       
	};
}
#endif
