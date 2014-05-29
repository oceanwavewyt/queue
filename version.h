#include "format.h"

namespace pile {
	class Version
	{
	public:
		static Version* Instance();
		Version();
		~Version();
		void Init(uint32_t blockid, uint32_t interid);
		void SetBlockId(uint32_t id);
		void SetBlockId();
		uint32_t GetBlockId();
		void SetBlockInterId();
		void GetCurrentId();
		uint32_t GetInterId();
	private:
		uint32_t blockid_;
		uint32_t interid_;
		static Version *instance_;	
	};
}