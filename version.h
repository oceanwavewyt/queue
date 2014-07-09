#include "format.h"

namespace levelque {
	class FixFile;
	class Version
	{
	public:
		static Version* Instance();
		Version();
		~Version();
		void SetFilelist(FixFile *f);
		void Init(uint32_t blockid, uint32_t interid);
		void SetBlockId(uint32_t id);
		void SetBlockId();
		uint32_t GetBlockId();
		void SetBlockInterId();
		void SetBlockOffset(uint64_t offset);
		void GetCurrentId();
		uint32_t GetInterId();
	private:
		uint32_t blockid_;
		uint32_t interid_;
		static Version *instance_;
		FixFile *filelist_;	
	};
}
