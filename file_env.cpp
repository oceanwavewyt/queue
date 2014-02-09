#include "file_env.h"
#include "version.h"
#include <math.h>

FixFile::FixFile(const std::string &fname, int fd):fd_(fd),
  fname_(fname),startid_(1),curFileid(0){

} 
bool FixFile::LoadFile(){
    struct stat s;
    stat(fname_.c_str(), &s);
    cout << fname_ << s.st_size << endl;
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
      curFileid = startid_;
      AddItem(curFileid);
  }
  QueueFileName::List(curFileid, filename);
  return 0;
}

FileId FixFile::GetCurrentFileId() {
  if(!curFileid) {
      curFileid = startid_;
      AddItem(curFileid);
  }
  return curFileid;
}

void FixFile::SetItemNumber(ItemNumber id) {
		

}

void FixFile::AddItem(FileId id) {
  for(int i=0; i<=fNum; i++){
	  uint32_t size = i*sizeof(fileList);
      fileList *file = reinterpret_cast<fileList *>(base_+size);
      if(file->status == fUse) {
          file->id = id;
          file->seq = TimeId(time(NULL));
          file->curpos = 0;
          file->status = fUsing;
		  break;
      }
  } 
}

void FixFile::GetUnUse(FILELIST &unuseFiles) {
    fileList *curfile=0;
    //cout << "fNum: " << fNum<< endl;
    for(int i=0; i<=fNum; i++){
        uint32_t size = i*sizeof(fileList);
		    fileList *file = reinterpret_cast<fileList *>(base_+size);
        //cout << "file status: "<< file->status << "id: " << file->id << endl;
		    //need delete file
        if(file->status == fUse) continue;
        if(file->status == fUsing) {
		      assert(curfile==0);
          curfile = file;
        }
        //insert 
        unuseFiles[file->seq] = static_cast<FileId>(file->id);
    }
    if(curfile) {
        curFileid = curfile->id;
    }
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
    Version::Instance()->SetBlockId();
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




