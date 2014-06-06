#include "format.h"
#include "file_env.h"
#include "reader.h"
#include "writer.h"
#include "version.h"
#include "util/crc32c.h"
#include "util/coding.h"

namespace levelque {
	Writer::Writer(MmapFile *dest): dest_(dest),block_offset_(0){
		for (int i = 0; i <= kMaxRecordType; i++) {
	   		 char t = static_cast<char>(i);
	    	type_crc_[i] = crc32c::Value(&t, 1);
	  }		
	}
	Writer::~Writer() {
		if(dest_) delete dest_;
	}

	void Writer::SetOffset(int fileOffset, int blockOffset) {
		dest_->Skip(fileOffset);
		block_offset_ = blockOffset;
	}

	bool Writer::AddRecord(const string &data, size_t length) {
		const char* ptr = data.c_str();
		size_t left = length;    
		bool begin = true;
		bool s = true;
		do {                                                                     
			const int leftover = kBlockSize - block_offset_;                       
			assert(leftover >= 0);                                                 
			if (leftover < kHeaderSize) {                                          
				//Switch to a new block                                             
				if (leftover > 0) {                                                  
					// Fill the trailer (literal below relies on kHeaderSize being 11)  
					assert(kHeaderSize == 11);                                          
					dest_->Append("\x00\x00\x00\x00\x00\x00", leftover);        
				}                                                                    
				block_offset_ = 0; 
				Version::Instance()->SetBlockId();                                                  
			}                                                                      

			// Invariant: we never leave < kHeaderSize bytes in a block.           
			assert(kBlockSize - block_offset_ - kHeaderSize >= 0);                 

			const size_t avail = kBlockSize - block_offset_ - kHeaderSize;         
			const size_t fragment_length = (left < avail) ? left : avail;    

			RecordType type;                                                       
			const bool end = (left == fragment_length);                            
			if (begin && end) {                                                    
				type = kFullType;                                                    
			} else if (begin) {                                                    
				type = kFirstType;                                                   
			} else if (end) {                                                      
				type = kLastType;                                                    
			} else {                                                               
				type = kMiddleType;                                                  
			}                                                                      		

			s = EmitPhysicalRecord(type, ptr, fragment_length);
				ptr += fragment_length;
				left -= fragment_length;
				begin = false;
			} while (s && left > 0);		
		return true;	
	}	

		
	bool Writer::EmitPhysicalRecord(RecordType t, const char* ptr, size_t n) {  
		assert(n <= 0xffff);  // Must fit in two bytes                              
		assert(block_offset_ + kHeaderSize + n <= kBlockSize);                      

		//Format the header                                                        
		char buf[kHeaderSize];                                                      
		buf[4] = static_cast<char>(n & 0xff);                                       
		buf[5] = static_cast<char>(n >> 8);                                         
		buf[6] = static_cast<char>(t);

		uint32_t bid = Version::Instance()->GetBlockId();                                              
		buf[7] = static_cast<char>(bid & 0xff);
		buf[8] = static_cast<char>(bid >> 8);
		uint32_t iid = Version::Instance()->GetInterId();
		buf[9] = static_cast<char>(iid & 0xff);
		buf[10] = static_cast<char>(iid >> 8);

		if(t==kLastType){
			cout << "write blockid: "<< bid << endl;
		}
		// Compute the crc of the record type and the payload.                      
		uint32_t crc = crc32c::Extend(0, ptr, n);                        
		crc = crc32c::Mask(crc);                 // Adjust for storage              
		EncodeFixed32(buf, crc);                                                    

		// Write the header and the payload                                       
		bool s = dest_->Append(buf, kHeaderSize);                          
		if (s) {
			s = dest_->Append(ptr, n);                                         
		}                                                                           
		block_offset_ += kHeaderSize + n;                                          
		return s;                                                                   
	}                                                                             
}
