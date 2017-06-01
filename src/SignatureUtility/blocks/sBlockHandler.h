#ifndef s_BLOCKHANDLER
#define s_BLOCKHANDLER

#include <queue>
#include <vector>
#include <memory>
#include "boost/thread.hpp"

#include "sBlock.h"
#include "help/sDefines.h"

class sBlockHandler
{
public:
	sBlockHandler(uint32 threadCount, size_t blockCount);
	~sBlockHandler();

	void startThreads();
	void interruptThreads();
	void rethrowException() const;
	void addBlockToProcess(std::shared_ptr<SBlock> block);

	std::string getHash(uint32 id);
	size_t      getOutputHashesSize();
	size_t      getInputSize();
private:
	void processBlock();
	void addToInput(std::shared_ptr<SBlock> block);
	void addToOutput(std::shared_ptr<SBlock> block);
private:
	uint32                     mThreadsCount;
	std::vector<boost::thread> mThreads;

	boost::mutex                        mInputMutex;
	std::queue<std::shared_ptr<SBlock>> mInputBlocks;

	boost::mutex             mOutputMutex;
	std::vector<std::string> mOutputHashes;

	uint32 mCurrentHashIndex;

	bool mIsStarted;
	bool mIsFinished;
	
	boost::mutex         mLastExceptionMutex;
	boost::exception_ptr mLastException;
};
#endif //s_BLOCKHANDLER