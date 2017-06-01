#ifndef sign_OPTIONS
#define sign_OPTIONS

#include <string>
#include "help/sDefines.h"

struct SOptions
{
	SOptions(std::string input, std::string output, size_t blockSize = 1000000) :
		mInputFile(input), mOutputFile(output), mBlockSize(blockSize) {}

	std::string  mInputFile;
	std::string  mOutputFile;
	size_t       mBlockSize;
};

#endif //sign_OPTIONS