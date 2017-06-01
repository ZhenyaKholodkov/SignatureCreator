#ifndef sign_PROGRAM
#define sign_PROGRAM

#include <memory>
#include <queue>

#include "sOptions.h"
#include "sBlock.h"
#include "sReadStream.h"
#include "sWriteStream.h"
#include "sBlockHandler.h"
#include "boost/thread.hpp"

class SProgram
{
public:
	SProgram(uint32 threadNum/* = 10*/);
	~SProgram();

	void runWithOptions(std::shared_ptr<SOptions> options);

private:
	void readBlocks(std::string path, size_t blockSize);
	void writeHashes(std::string path);
	void interruptHandlerThreads();

private:
	const uint32 DEFAULT_THREADS_NUM_FOR_HASH;

	boost::mutex                   mBlockHandlerMutex;
	std::unique_ptr<sBlockHandler> mBlockHandler;
	std::shared_ptr<SOptions>      mOptions;
};
#endif //sign_PROGRAM

