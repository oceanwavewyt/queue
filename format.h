#ifndef FORMAT_H_
#define FORMAT_H_

#include <deque>
#include <set>
#include <iostream>
#include <string>
#include <map>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <inttypes.h>
#include <time.h>
#include <unistd.h>
#include <sys/stat.h>
#include <assert.h>
#include <time.h>

#include <iostream>
#include <fstream>

using namespace std;

enum RecordType {
	// Zero is reserved for preallocated files
	kZeroType = 0,
	kFullType = 1,
	// For fragments
	kFirstType = 2,
	kMiddleType = 3,
	kLastType = 4
};

static const int kMaxRecordType = kLastType;

static const uint32_t kBlockSize = 32768;

// Header is checksum (4 bytes), type (1 byte), length (2 bytes), itemid(4 bytes).
static const uint8_t kHeaderSize = 4 + 1 + 2 + 4;

enum FileStatus {
	//the files that not are used
	fUnUse = 1,
	//the files used,already readed
	fUse = 0,
	//the file using
	fUsing = 2
};

static const int fNum = 100;
static const uint32_t fMaxBlockNum = 10; //65535;

//memory buffer size
static const uint64_t mMaxBufferSize = 1024*1024*1; //10M

typedef uint16_t FileId;
typedef uint32_t ItemNumber;
typedef uint64_t TimeId;

typedef struct filepos{
    FileId id;
    uint32_t blockid;
	uint32_t offset;
	ItemNumber curpos;
	struct filepos& operator= (struct filepos &s) {
		id = s.id;
		blockid = s.blockid;
		offset = s.offset;
		curpos = s.curpos;
		return *this;
	}
}filePos;
typedef std::map<TimeId, filePos> FILELIST; 

class QueueFileName
{
public:
	static void List(FileId id, std::string &filename) {
		char buf[50] = {0};
		sprintf(buf,"./data/queue_%04d.lst", id);
		filename = buf;
	}  	

};


#endif
