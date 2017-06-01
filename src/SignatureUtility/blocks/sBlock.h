#ifndef sign_BLOCK
#define sign_BLOCK

#include <string>
#include "help/sDefines.h"

struct SBlock
{
	SBlock(size_t blockSize)  : mId(0)
	{
		mData.resize(blockSize);
		std::fill(mData.begin(), mData.end(), '0');
	}
	bool isHashReady() const { return !mHash.empty(); }

	uint32 mId;
	std::string mData;
	std::string mHash;
};

#endif //sign_OPTIONS