#include "reader.h"
#include "file_env.h"
#include "version.h"

Reader::Reader(SequentialFile* file, bool checksum,
               uint64_t initial_offset)
    : file_(file),
      checksum_(checksum),
      backing_store_(new char[kBlockSize]),
      buffer_(),
      eof_(false),
      last_record_offset_(0),
	  end_of_buffer_offset_(0),
	  read_block_num_(-1),
	  real_read_block_num_(0),
      last_record_end_offset_(0),
	  offset_in_block_(0),
      initial_offset_(initial_offset) {
}

Reader::~Reader() {
  delete[] backing_store_;
}

bool Reader::SkipToInitialBlock() {
  offset_in_block_ = initial_offset_ % kBlockSize;
  uint64_t block_start_location = initial_offset_ - offset_in_block_;

  // Don't search a block if we'd be in the trailer
  if (offset_in_block_ > kBlockSize - 6) {
    offset_in_block_ = 0;
    block_start_location += kBlockSize;
  }

  end_of_buffer_offset_ = block_start_location;

  // Skip to start of first block that can contain the initial record
  if (block_start_location > 0) {
    bool s = file_->Skip(block_start_location);
    if (!s) {
      ReportDrop(block_start_location);
      return false;
    }
  }

  return true;
}

uint32_t Reader::ReadRecord(string &record, std::string &scratch, uint32_t &blockid) {
  if (!SkipToInitialBlock()) {
      cout << "SkipToInitialBlock failed" << endl;
      return 0;
  }
  initial_offset_ = 0;
  scratch.clear();
  record.clear();
  bool in_fragmented_record = false;
  // Record offset of the logical record that we're reading
  // 0 is a dummy value to make compilers happy
  uint64_t prospective_record_offset = 0;

  string fragment;
  fragment.clear();
  while (true) {
	uint64_t physical_record_offset = end_of_buffer_offset_ - buffer_.size();
	uint32_t id;
    const unsigned int record_type = ReadPhysicalRecord(fragment, id, blockid);
    //cout << "record_type: " << record_type << endl;
    switch (record_type) {
      case kFullType:
        if (in_fragmented_record) {
          // Handle bug in earlier versions of log::Writer where
          // it could emit an empty kFirstType record at the tail end
          // of a block followed by a kFullType or kFirstType record
          // at the beginning of the next block.
          if (scratch.empty()) {
            in_fragmented_record = false;
          } else {
            ReportCorruption(scratch.size(), "partial record without end(1)");
          }
        }
        prospective_record_offset = physical_record_offset;
        scratch.clear();
        record = fragment;
        last_record_offset_ = prospective_record_offset;
        last_record_end_offset_ = kBlockSize - buffer_.size();
		if(read_block_num_ != -1) {
			real_read_block_num_ = read_block_num_;
		}
		//Version::Instance()->SetBlockInterId();
        //cout << "kFullType: " << last_record_end_offset_ << endl;
        return id;

      case kFirstType:
        if (in_fragmented_record) {
          // Handle bug in earlier versions of log::Writer where
          // it could emit an empty kFirstType record at the tail end
          // of a block followed by a kFullType or kFirstType record
          // at the beginning of the next block.
          if (scratch.empty()) {
            in_fragmented_record = false;
          } else {
            ReportCorruption(scratch.size(), "partial record without end(2)");
          }
        }
        prospective_record_offset = physical_record_offset;
        scratch.assign(fragment.data(), fragment.size());
        in_fragmented_record = true;
        break;

      case kMiddleType:
        if (!in_fragmented_record) {
          ReportCorruption(fragment.size(),
                           "missing start of fragmented record(1)");
        } else {
          scratch.append(fragment.data(), fragment.size());
        }
        break;

      case kLastType:
        if (!in_fragmented_record) {
          ReportCorruption(fragment.size(),
                           "missing start of fragmented record(2)");
        } else {
          scratch.append(fragment.data(), fragment.size());
          record = scratch;
          last_record_offset_ = prospective_record_offset;
          last_record_end_offset_ = kBlockSize - buffer_.size();
		  if(read_block_num_ != -1) {
			real_read_block_num_ = read_block_num_;
		  }
		  //Version::Instance()->SetBlockInterId();	
          return id;
        }
        break;

      case kEof:
        if (in_fragmented_record) {
          ReportCorruption(scratch.size(), "partial record without end(3)");
          scratch.clear();
        }
        return 0;

      case kBadRecord:
        if (in_fragmented_record) {
          ReportCorruption(scratch.size(), "error in middle of record");
          in_fragmented_record = false;
          scratch.clear();
        }
        break;

      default: {
        char buf[40];
        snprintf(buf, sizeof(buf), "unknown record type %u", record_type);
        ReportCorruption(
            (fragment.size() + (in_fragmented_record ? scratch.size() : 0)),
            buf);
        in_fragmented_record = false;
        scratch.clear();
        break;
      }
    }
  }
  return 0;
}

uint64_t Reader::LastRecordOffset() {
  return last_record_offset_;
}

uint64_t Reader::FileEndOffset() {
 	return real_read_block_num_*kBlockSize + last_record_end_offset_; 
}

uint64_t Reader::BlockEndOffset() {
	return last_record_end_offset_;
}


void Reader::ReportCorruption(size_t bytes, const char* reason) {
  //ReportDrop(bytes, Status::Corruption(reason));
}

void Reader::ReportDrop(size_t bytes) {
  //if (reporter_ != NULL &&
  //    end_of_buffer_offset_ - buffer_.size() - bytes >= initial_offset_) {
    //reporter_->Corruption(bytes, reason);
  //}
}

unsigned int Reader::ReadPhysicalRecord(string &result, uint32_t &id, uint32_t &blockid) {
  while (true) {
    //cout << "buffer_.size: "<< buffer_.size() << endl;
    if (buffer_.size() < kHeaderSize) {
      if (!eof_) {
        // Last read was a full read, so this is a trailer to skip
        buffer_.clear();
        bool s = file_->Read(kBlockSize, buffer_, backing_store_);
        end_of_buffer_offset_ += buffer_.size();
		    read_block_num_++;
		    //Version::Instance()->SetBlockId();
        if (!s) {
          buffer_.clear();
          ReportDrop(kBlockSize);
          eof_ = true;
          return kEof;
        } else if (buffer_.size() < kBlockSize) {
          eof_ = true;
        }
        continue;
      } else if (buffer_.size() == 0) {
        // End of file
        return kEof;
      } else {
        //size_t drop_size = buffer_.size();
        buffer_.clear();
        return kEof;
      }
    }

    if(offset_in_block_ > 0) {
        buffer_ = buffer_.substr(offset_in_block_);
        offset_in_block_ = 0;
    }
    // Parse the header
    const char* header = buffer_.c_str();
    const uint32_t a = static_cast<uint32_t>(header[4]) & 0xff;
    const uint32_t b = static_cast<uint32_t>(header[5]) & 0xff;
    const unsigned int type = header[6];
    const uint32_t length = a | (b << 8);
    //blockid
    const uint32_t c = static_cast<uint32_t>(header[7]) & 0xff;
    const uint32_t d = static_cast<uint32_t>(header[8]) & 0xff;
    const uint32_t bid = c | (d << 8);
    //interid
    const uint32_t e = static_cast<uint32_t>(header[9]) & 0xff;
    const uint32_t f = static_cast<uint32_t>(header[10]) & 0xff;
    const uint32_t iid = e | (f << 8);

    id = iid | (bid << 16);
	  blockid = bid;
	//cout << "bbbbbbbbid: " << bid  << "\tid: " << id << endl;
    if (kHeaderSize + length > buffer_.size()) {
      size_t drop_size = buffer_.size();
      buffer_.clear();
      cout << drop_size << " bad record length" << endl;
      return kBadRecord;
    }

    if (type == kZeroType && length == 0) {
      // Skip zero length record without reporting any drops since
      // such records are produced by the mmap based writing code in
      // env_posix.cc that preallocates file regions.
      buffer_.clear();
      return kBadRecord;
    }

	buffer_ = buffer_.substr(kHeaderSize + length);	

    //buffer_.remove_prefix(kHeaderSize + length);

    // Skip physical record that started before initial_offset_
    if (end_of_buffer_offset_ - buffer_.size() - kHeaderSize - length <
        initial_offset_) {
      result.clear();
      return kBadRecord;
    }

	result.assign(header + kHeaderSize, length);
    return type;
  }
}

