#include "sBlockHandler.h"

#include "sha256.h"

sBlockHandler::sBlockHandler(uint32 threadCount, size_t blockCount) :
	mThreadsCount(threadCount),
	mCurrentHashIndex(0),
	mIsStarted(false),
	mIsFinished(false), 
	mLastException(nullptr)
{
	mOutputHashes.resize(blockCount);
}

sBlockHandler::~sBlockHandler()
{
}

void sBlockHandler::startThreads()
{
	for (uint32 i = 0; i < mThreadsCount; ++i)
	{
		mThreads.push_back(boost::thread(&sBlockHandler::processBlock, this));
	}
	mIsStarted = true;
}

void sBlockHandler::interruptThreads()
{
	for (uint32 i = 0; i < mThreadsCount; ++i)
	{
		mThreads[i].interrupt();
	}
}

void sBlockHandler::rethrowException() const
{
	if (mLastException)
	{
		boost::rethrow_exception(mLastException);
	}
}

void sBlockHandler::addBlockToProcess(std::shared_ptr<SBlock> block)
{
	addToInput(block);
	if(!mIsStarted)   
		startThreads();      // we got a first block to handle, start the threads 
}

void sBlockHandler::addToInput(std::shared_ptr<SBlock> block)
{
	boost::lock_guard<boost::mutex> inputLock(mInputMutex);
	mInputBlocks.push(block);
}

void sBlockHandler::addToOutput(std::shared_ptr<SBlock> block)
{
	boost::lock_guard<boost::mutex> outputLock(mOutputMutex);
	mOutputHashes[mCurrentHashIndex++] = block->mHash;
	if (mCurrentHashIndex == mOutputHashes.size()) // all blocks were handled
	{
		boost::lock_guard<boost::mutex> locker(mInputMutex);
		mIsFinished = true;
	}
}

void sBlockHandler::processBlock()
{
	while (1)
	{
		try
		{
			boost::this_thread::interruption_point();
			std::shared_ptr<SBlock> block;
			{
				boost::lock_guard<boost::mutex> locker(mInputMutex);
				if (!mInputBlocks.empty())
				{
					block = mInputBlocks.front();
					mInputBlocks.pop();
				}
				else if (mIsFinished)
				{
					break;
				}
			}
			if (block)
			{
				block->mHash = sha256(block->mData);
				addToOutput(block);
			}
		}
		catch (...)
		{
			boost::lock_guard<boost::mutex> locker(mLastExceptionMutex);
			mLastException = boost::current_exception();                  //remeber the exception to rethrow it in Program class
		}
	}
}

std::string sBlockHandler::getHash(uint32 id)
{
	std::string hash;
	boost::lock_guard<boost::mutex> locker(mOutputMutex);
	if (!mOutputHashes[id].empty())
	{
		hash = mOutputHashes[id];
	}
	return hash;
}

size_t sBlockHandler::getOutputHashesSize()
{
	boost::lock_guard<boost::mutex> locker(mOutputMutex);
	return mOutputHashes.size();
}

size_t sBlockHandler::getInputSize()
{
	boost::lock_guard<boost::mutex> locker(mInputMutex);
	return mInputBlocks.size();
}