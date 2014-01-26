#ifndef FILE_ENV_H_
#define FILE_ENV_H_

#include "format.h"

using namespace std;


class SequentialFile {
	std::string filename_;
	FILE* file_;
public:                                                        
	SequentialFile(const std::string& fname, FILE* f);
	~SequentialFile();            
	bool Read(size_t n, string &result, char* scratch);
	bool Skip(uint64_t n);                              
};

class MmapFile
{
private:
  std::string filename_;
  int fd_;
  size_t page_size_;
  size_t map_size_;       // How much extra memory to map at a time
  char* base_;            // The mapped region
  char* limit_;           // Limit of the mapped region
  char* dst_;             // Where to write next  (in range [base_,limit_])
  char* last_sync_;       // Where have we synced up to
  uint64_t file_offset_;  // Offset of base_ in file

  // Have we done an munmap of unsynced data?
  bool pending_sync_;

public:
	MmapFile(const std::string& fname, int fd, size_t page_size);
  	~MmapFile();
  static size_t Roundup(size_t x, size_t y);
  bool Append(const char *data, size_t length);
private:
  bool UnmapCurrentRegion();
  bool MapNewRegion();
  bool Close();
};


class FixFile
{
  typedef struct filelist{
    FileId id;
    TimeId seq;
    ItemNumber curpos;
    FileStatus status;
  }fileList;

public:
  FixFile(const std::string &fname, int fd);
  ~FixFile();
  int GetCurrentFile(std::string &filename);
  bool LoadFile();
  //按照时间顺序读取未使用过的文件列表
  void GetUnUse(FILELIST &unuseFiles);
  void SetItemNumber(ItemNumber id);
  void AddItem(FileId id);
private:
  int fd_;
  std::string fname_;
  char *base_;
  int startid_;
  FileId curFileid;
};

class Files
{
	size_t page_size_;
public:
	Files() : page_size_(getpagesize()) {
	}
	~Files(){
	}

	bool NewWritableFile(const std::string& fname, MmapFile** result) {
		const int fd = open(fname.c_str(), O_CREAT | O_RDWR | O_TRUNC, 0644);
		if (fd < 0) {
			*result = NULL;
			return false;
		} else {
			*result = new MmapFile(fname, fd, page_size_);
		}
		return true;
	}
	
	bool NewSequentialFile(const std::string& fname,
                                 SequentialFile** result) {
  		FILE* f = fopen(fname.c_str(), "r");
  		if (f == NULL) {
    		*result = NULL;
    		return false;
  		} else {
    		*result = new SequentialFile(fname, f);
    		return true;
  		}
	}

  bool NewFixFile(const std::string & fname, FixFile** result) {
    int f = O_RDWR;
    if(access(fname.c_str(),0) == -1) {
      f = f|O_CREAT;
    }
    const int fd = open(fname.c_str(), f, 0644);
    if (fd < 0) {
      *result = NULL;
      return false;
    } else {
      *result = new FixFile(fname, fd);
    }
    return true;

  }

};
#endif
