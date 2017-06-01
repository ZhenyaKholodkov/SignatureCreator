#include "sProgram.h"

#include <vector>
#include "boost/chrono.hpp"



SProgram::SProgram(uint32 threadNum = 10) :
	DEFAULT_THREADS_NUM_FOR_HASH(threadNum)
{
}


SProgram::~SProgram()
{
}


void SProgram::runWithOptions(std::shared_ptr<SOptions> options)
{
	/*just calculate the time*/
	auto time1 = boost::chrono::high_resolution_clock::now();

	mOptions = options;
	boost::thread readThread(&SProgram::readBlocks, this, options->mInputFile, options->mBlockSize);
	printf("wait...\n");
	readThread.join();

	auto time2 = boost::chrono::high_resolution_clock::now();
	auto spentTime = boost::chrono::duration_cast<boost::chrono::milliseconds>(time2 - time1);

	printf("Done. Spent time - %lld\n", spentTime.count());
}
/*
*       reads blocks and passes them to BlockHandler to calculate hashes
*       starts threads of BlockHandler and the thread for writing hashes
*/
void SProgram::readBlocks(std::string path, size_t blockSize)
{
	std::unique_ptr<SReadStream> mFileReader = std::make_unique<SReadStream>(mOptions->mInputFile, mOptions->mBlockSize);
	size_t fileSize = mFileReader->openFile();
	if (fileSize == 0)
	{
		printf("File was not opened for reading.\n");
		return;
	}

	{
		boost::lock_guard<boost::mutex> lock(mBlockHandlerMutex);
		double blockCount = std::ceil((static_cast<double>(fileSize) / static_cast<double>(mOptions->mBlockSize))); 
		mBlockHandler = std::make_unique<sBlockHandler>(DEFAULT_THREADS_NUM_FOR_HASH, static_cast<size_t>(blockCount));
	}
	boost::thread writeThread(&SProgram::writeHashes, this, mOptions->mOutputFile);

	bool needWait = false;
	while (!mFileReader->isFileEnd())
	{
		try
		{
			std::shared_ptr<SBlock> block = mFileReader->readNextBlock();
			{
				boost::lock_guard<boost::mutex> lock(mBlockHandlerMutex);
				mBlockHandler->addBlockToProcess(block);
				if (mBlockHandler->getInputSize() > DEFAULT_THREADS_NUM_FOR_HASH * 2) // waiting till BlockHander calculates hashes and WriteThread writes them
				{                                                                     // for not clogging the memory 
					needWait = true;
				}
				else
				{
					needWait = false;
				}
			}
			if (needWait)
			{
				boost::this_thread::sleep(boost::posix_time::milliseconds(10));
			}
		}
		catch (const std::ifstream::failure exception)
		{
			printf("Error occured while reading the block.\n");
			writeThread.interrupt();
			interruptHandlerThreads();
			break;
		}
	}
	writeThread.join();
}
/*
*    writes hashes to a file
*
*/
void SProgram::writeHashes(std::string path)
{
	std::unique_ptr<SWriteStream> fileWriter = std::make_unique<SWriteStream>(mOptions->mOutputFile);
	if (!fileWriter->openFile())
	{
		printf("File was not opened for writing.");
		interruptHandlerThreads();
		return;
	}

	uint32 blockId = 0;
	while (1)
	{
		try
		{
			boost::this_thread::interruption_point();
			boost::unique_lock<boost::mutex> locker(mBlockHandlerMutex);
			mBlockHandler->rethrowException();     //if some thread of the BlockHandler threw an exception, rethrow and handle it
			if (mBlockHandler->getOutputHashesSize() == blockId) //all blocks were handled 
			{
				break;
			}
			std::string hash= mBlockHandler->getHash(blockId);
			locker.unlock();
			if (!hash.empty())
			{
				fileWriter->writeToFile(hash);
				blockId++;
			}
		}
		catch (const std::ifstream::failure exception)
		{
			printf("Error occured while writing the block.\n");
			interruptHandlerThreads();
			return;

		}
		catch (const std::exception exception)
		{
			printf("Exception was occured! %s", exception.what());
			interruptHandlerThreads();
			return;
		}
	}
}

/*
*     interrupts threads of BlockHandler
*/
void SProgram::interruptHandlerThreads()
{
	boost::lock_guard<boost::mutex> lock(mBlockHandlerMutex);
	if(mBlockHandler) mBlockHandler->interruptThreads();
}