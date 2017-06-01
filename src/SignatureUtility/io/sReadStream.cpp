#include "sReadStream.h"

#include <Windows.h>

SReadStream::SReadStream(std::string path, size_t blockSize) :
	mFilePath(path),
	mBlockSize(blockSize),
	mIndex(0)
{
}

SReadStream::~SReadStream()
{
	if (mFileReader.is_open())
		mFileReader.close();
}

size_t SReadStream::openFile()
{
	mFileReader.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try
	{
		mFileReader.open(mFilePath.c_str());
		
		mFileReader.seekg(0, std::ios::end);
		mFileSize = mFileReader.tellg();
		mFileReader.seekg(0, std::ios::beg);
	}
	catch (std::ifstream::failure exception)
	{
		perror(("Error while opening the file occured. Exception - %s", exception.what()));
		return 0;
	}
	return mFileSize;
}

std::shared_ptr<SBlock> SReadStream::readNextBlock()
{
	try
	{
		if (!mFileReader.is_open())
		{
			throw std::ios::ios_base::failure("File is closed!\n");
		}
		size_t readSize = mBlockSize;
		if (mFileSize < mBlockSize)
			readSize = mFileSize;

		std::shared_ptr<SBlock> block = std::make_shared<SBlock>(mBlockSize);
		block->mId = mIndex++;
		mFileReader.rdbuf()->sgetn(&block->mData[0], readSize);
		mFileSize -= readSize;
		return block;
	}
	catch (std::ifstream::failure exception)
	{
		perror(("Error while reading the file occured. Exception - %s", exception.what()));
		if (mFileReader.is_open())
			mFileReader.close();
		throw;
	}
}

bool SReadStream::isFileEnd() const
{
	return mFileSize == 0;
}