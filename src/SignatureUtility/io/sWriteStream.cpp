#include "sWriteStream.h"


SWriteStream::SWriteStream(std::string path) : mFilePath(path) 
{
}

SWriteStream::~SWriteStream()
{
	if (mFileWriter.is_open())
		mFileWriter.close();
}

bool SWriteStream::openFile()
{
	mFileWriter.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try
	{
		mFileWriter.open(mFilePath.c_str());
	}
	catch (std::ifstream::failure exception)
	{
		perror(("Error while opening the file occured. Exception - %s", exception.what()));
		return false;
	}
	return true;
}

void SWriteStream::writeToFile(std::string hash)
{
	try
	{
		if (!mFileWriter.is_open())
		{
			throw std::ios::ios_base::failure("File is closed!\n");
		}

		mFileWriter.write(&hash[0], hash.size());
	}
	catch (std::ifstream::failure exception)
	{
		perror(("Error while reading the file occured. Exception - %s", exception.what()));
		mFileWriter.close();
		throw;
	}
}