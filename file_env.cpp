#include "format.h"
#include "file_env.h"
#include "version.h"
#include <math.h>

namespace pile {
  FixFile::FixFile(const std::string &fname, int fd, uint8_t level):fd_(fd),
    fname_(fname),startid_(1),curFileid(0),level_(level){

  } 
  bool FixFile::LoadFile(){
      struct stat s;
      stat(fname_.c_str(), &s);
  	if(s.st_size  <= 0){
        if (ftruncate(fd_, sizeof(fileList)*fNum) < 0) {
          return false;
        }
      }
      void* ptr = mmap(NULL, sizeof(fileList)*fNum, PROT_READ | PROT_WRITE, MAP_SHARED,
                       fd_, 0);
      if (ptr == MAP_FAILED) {
        return false;
      }
      base_ = reinterpret_cast<char*>(ptr);
      return true;
  }

  FixFile::~FixFile(){
      munmap(base_, sizeof(fileList)*fNum);
      close(fd_);
  }                                              

  int FixFile::GetCurrentFile(std::string &filename) {
    if(!curFileid) {
  	  curFileid = GetCurrentFileId();
    }
    QueueFileName::List(curFileid, filename, level_);
    //filename = Opt::GetBasePath() + "/" + filename;
    return 0;
  }

  FileId FixFile::GetCurrentFileId() {
  	if(curFileid) return curFileid;
  	int firstUse = -1;
  	for(int i=0; i<=fNum; i++){                                   
  		uint32_t size = i*sizeof(fileList);                       
      	fileList *file = reinterpret_cast<fileList *>(base_+size);
      	if(file->status == fUse && firstUse == -1) {
  			firstUse = i;
  		}                       
          if(file->status == fUsing) {
  			return file->id;
  		}                                                                   
  	}                                                             	
  	//no find and set Using
  	assert(firstUse!=-1);
  	uint32_t size = firstUse*sizeof(fileList);
  	fileList *file = reinterpret_cast<fileList *>(base_+size);
  	file->id = firstUse+1;
  	//if file exists and delete file
  	string filename;
  	QueueFileName::List(file->id, filename, level_);
  	//filename = Opt::GetBasePath() + "/" + filename;
  	if(access(filename.c_str(), F_OK)>=0) {
  		assert(remove(filename.c_str())==0);	
  	}		
      file->seq = TimeId(time(NULL));
      file->blockid = 0;
  	file->offset = 0;
  	file->curpos = 0;
      file->status = fUsing;
  	return file->id;
  }

  void FixFile::SetItemNumber(FileId fid, uint32_t blockid, uint64_t blockOffset, ItemNumber id) {
  	if(fid <=0) return;
  	uint32_t size = (fid-1)*sizeof(fileList);                
  	fileList *file = reinterpret_cast<fileList *>(base_+size);	
  	file->blockid = blockid;
  	file->offset = blockOffset;	
  	file->curpos = id;
  }


  void FixFile::ReleaseCurFile() {
  	if(curFileid == 0) return;
   	cout << "curFileid:" << curFileid <<"\tfNum:"<<fNum<<endl;
  	assert(curFileid<=fNum);
  	uint32_t s = (curFileid-1)*sizeof(fileList);
  	fileList *file = reinterpret_cast<fileList *>(base_+s);
  	file->status = fUnUse;  				
  	curFileid = 0;
  }

  //set status from fUnUse to fUse
  bool FixFile::SetUse(FileId fid) {
  	if(fid <=0) return false;
  	uint32_t s = (fid-1)*sizeof(fileList);
  	fileList *file = reinterpret_cast<fileList *>(base_+s);
  	file->status = fUse;
  	return true;
  }


  void FixFile::GetUnUse(FILELIST &unuseFiles) {
      fileList *curfile=0;
      //cout << "fNum: " << fNum<< endl;
      for(int i=0; i<=fNum; i++){
          uint32_t size = i*sizeof(fileList);
  		fileList *file = reinterpret_cast<fileList *>(base_+size);
          if(file->id) {
  			//cout << "file status: "<< file->status << " id: " << file->id << "  blockid: "<<file->blockid << " pos: "<<file->curpos << endl;
  		}
  		    //need delete file
          if(file->status == fUse) continue;
          if(file->status == fUsing) {
  		      assert(curfile==0);
            curfile = file;
          }
          //insert 
         	filePos fpos;
  		fpos.id = static_cast<FileId>(file->id);
  		fpos.blockid = file->blockid;
  		fpos.offset = file->offset;
  		fpos.curpos = file->curpos;
  		unuseFiles[file->seq] = fpos;
      }
      if(curfile) {
          curFileid = curfile->id;
      }
  }


  LevelFile::LevelFile(const std::string &fname, int fd):fd_(fd),
    fname_(fname){

  } 
  bool LevelFile::LoadFile(){
      struct stat s;
      stat(fname_.c_str(), &s);
    if(s.st_size  <= 0){
        if (ftruncate(fd_, sizeof(levelItem)*levelNum) < 0) {
          return false;
        }
      }
      base_ = (levelItem *)mmap(NULL, sizeof(levelItem)*levelNum, PROT_READ | PROT_WRITE, MAP_SHARED,
                       fd_, 0);
      if (base_ == MAP_FAILED) {
        return false;
      }
      return true;
  }

  LevelFile::~LevelFile(){
      munmap(base_, sizeof(levelItem)*levelNum);
      close(fd_);
  }              

  void LevelFile::AddItemNumber(uint8_t levelid) {
       Lock lock(&levelLock_);
      if(levelid > levelNum) return;
      base_[levelid].num++;       
  }

  void LevelFile::SubItemNumber(uint8_t levelid) {
      Lock lock(&levelLock_);
      if(levelid > levelNum) return;
      base_[levelid].num--;
  }

  SequentialFile::SequentialFile(const std::string& fname, FILE* f)
  	: filename_(fname), file_(f) { 
  }
                            
  SequentialFile::~SequentialFile() { 
  	fclose(file_);
  }
  bool SequentialFile::Read(size_t n, string &result, char* scratch) {                                                    
  	size_t r = fread(scratch, 1, n, file_);            
  	result.assign(scratch,r);                                
  	if (r < n) {                                                
  		if (feof(file_)) {                                        
  			//We leave status as ok if we hit the end of the file  
  		} else {                                                  
  			// A partial read with an error: return a non-ok status 
  			return false;
  		}                                                         
  	}                                                           
  	return true;                                                   
  }                                                             

  bool SequentialFile::Skip(uint64_t n) {                             
  	if (fseek(file_, n, SEEK_CUR)) {                            
  		return false;		
  	}                
  	return true;                                           
  }                                                             



  MmapFile::MmapFile(const std::string& fname, int fd, size_t page_size)
        : filename_(fname),
          fd_(fd),
          page_size_(page_size),
          map_size_(Roundup(65536, page_size)),
          base_(NULL),
          limit_(NULL),
          dst_(NULL),
          last_sync_(NULL),
          file_offset_(0),
          skip_size_(0),
          pending_sync_(false) {
      assert((page_size & (page_size - 1)) == 0);
  }


  MmapFile::~MmapFile() {
      if (fd_ >= 0) {
        MmapFile::Close();
      }
  }

  bool MmapFile::Skip(uint64_t n) {                             
    skip_size_ = n;             
    return true;                                           
  } 
  size_t MmapFile::Roundup(size_t x, size_t y) {
    	return ((x + y - 1) / y) * y;
   } 	

  size_t MmapFile::GetAvail(size_t length) {
      return limit_ - dst_;
  }

  bool MmapFile::Append(const char *data, size_t length) {
      const char* src = data;
      size_t left = length;
      while (left > 0) {
        assert(base_ <= dst_);
        assert(dst_ <= limit_);
        size_t avail = limit_ - dst_;
        if (avail == 0) {
          if (!UnmapCurrentRegion() ||
              !MapNewRegion()) {
  		  cout << "New region failed." << endl;
            return false;
          }
        }

        size_t n = (left <= avail) ? left : avail;
        memcpy(dst_, src, n);
        dst_ += n;
        src += n;
        left -= n;
      }
      return true;
  }

  bool MmapFile::UnmapCurrentRegion() {
      bool result = true;
      if (base_ != NULL) {
        if (last_sync_ < limit_) {
          // Defer syncing this data until next Sync() call, if any
          pending_sync_ = true;
        }
        if (munmap(base_, limit_ - base_) != 0) {
          result = false;
        }
        file_offset_ += limit_ - base_;
        base_ = NULL;
        limit_ = NULL;
        last_sync_ = NULL;
        dst_ = NULL;

        // Increase the amount we map the next time, but capped at 1MB
        if (map_size_ < (1<<20)) {
          map_size_ *= 2;
        }
      }
      return result;
    }

  bool MmapFile::MapNewRegion() {
      assert(base_ == NULL);
      if(skip_size_ > 0) {
        file_offset_ = floor(skip_size_/page_size_)*page_size_;
        skip_size_ = skip_size_-file_offset_;
      }
  	if (ftruncate(fd_, file_offset_ + map_size_) < 0) {
        return false;
      }
      void* ptr = mmap(NULL, map_size_, PROT_READ | PROT_WRITE, MAP_SHARED,
                       fd_, file_offset_);
      if (ptr == MAP_FAILED) {
        return false;
      }
      //Version::Instance()->SetBlockId();
      base_ = reinterpret_cast<char*>(ptr);
      limit_ = base_ + map_size_;
      dst_ = base_;
  	if(skip_size_ > 0) {
  		dst_ += skip_size_;
  		skip_size_ = 0;
  	}
      last_sync_ = base_;
      return true;
    }


  bool MmapFile::Close() {
      size_t unused = limit_ - dst_;
      if (!UnmapCurrentRegion()) {
        return false;
      } else if (unused > 0) {
        // Trim the extra space at the end of the file
        if (ftruncate(fd_, file_offset_ - unused) < 0) {
          return false;
        }
      }

      if (close(fd_) < 0) {
       	return false; 
      }

      fd_ = -1;
      base_ = NULL;
      limit_ = NULL;
      return true;
  }

}
