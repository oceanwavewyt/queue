// Copyright (c) 2011 The LevelDB Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#ifndef READER_H_
#define READER_H_

#include <stdint.h>
#include "file_env.h"

#include "format.h"

class Reader {
 public:
  // Interface for reporting errors.
  class Reporter {
   public:
    virtual ~Reporter();

    // Some corruption was detected.  "size" is the approximate number
    // of bytes dropped due to the corruption.
    virtual void Corruption(size_t bytes) = 0;
  };

  // Create a reader that will return log records from "*file".
  // "*file" must remain live while this Reader is in use.
  //
  // If "reporter" is non-NULL, it is notified whenever some data is
  // dropped due to a detected corruption.  "*reporter" must remain
  // live while this Reader is in use.
  //
  // If "checksum" is true, verify checksums if available.
  //
  // The Reader will start reading at the first record located at physical
  // position >= initial_offset within the file.
  Reader(SequentialFile* file, bool checksum,
         uint64_t initial_offset);

  ~Reader();

  // Read the next record into *record.  Returns true if read
  // successfully, false if we hit end of the input.  May use
  // "*scratch" as temporary storage.  The contents filled in *record
  // will only be valid until the next mutating operation on this
  // reader or the next mutation to *scratch.
  uint32_t ReadRecord(string &record, std::string& scratch, uint32_t &blockid);

  // Returns the physical offset of the last record returned by ReadRecord.
  //
  // Undefined before the first call to ReadRecord.
  uint64_t LastRecordOffset();
  // Offset of file wrote
  uint64_t FileEndOffset();
  // Offset of block position
  uint64_t BlockEndOffset();
 private:
  SequentialFile* const file_;
  //Reporter* const reporter_;
  bool const checksum_;
  char* const backing_store_;
  string buffer_;
  bool eof_;   // Last Read() indicated EOF by returning < kBlockSize

  // Offset of the last record returned by ReadRecord.
  uint64_t last_record_offset_;
  // Offset of the first location past the end of buffer_.
  uint64_t end_of_buffer_offset_;
  // The number of reading block
  int read_block_num_;
  // This is read_block_num_-1 
  int real_read_block_num_;
  // Offset of the last block 
  uint64_t last_record_end_offset_;

  // Offset of the first block	
  size_t offset_in_block_;
  // Offset at which to start looking for the first record to return
  uint64_t initial_offset_;
  	
  // Extend record types with the following special values
  enum {
    kEof = kMaxRecordType + 1,
    // Returned whenever we find an invalid physical record.
    // Currently there are three situations in which this happens:
    // * The record has an invalid CRC (ReadPhysicalRecord reports a drop)
    // * The record is a 0-length record (No drop is reported)
    // * The record is below constructor's initial_offset (No drop is reported)
    kBadRecord = kMaxRecordType + 2
  };

  // Skips all blocks that are completely before "initial_offset_".
  //
  // Returns true on success. Handles reporting.
  bool SkipToInitialBlock();

  // Return type, or one of the preceding special values
  unsigned int ReadPhysicalRecord(string &result, uint32_t &id, uint32_t &blockid);

  // Reports dropped bytes to the reporter.
  // buffer_ must be updated to remove the dropped bytes prior to invocation.
  void ReportCorruption(size_t bytes, const char* reason);
  void ReportDrop(size_t bytes );

  // No copying allowed
  Reader(const Reader&);
  void operator=(const Reader&);
};

#endif  // STORAGE_LEVELDB_DB_LOG_READER_H_
