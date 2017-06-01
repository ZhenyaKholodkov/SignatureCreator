#ifndef sign_READ_STREAM
#define sign_READ_STREAM

#include <exception>
#include  <fstream>
#include  <memory>

#include "sBlock.h"


class SReadStream
{
public:
	SReadStream(std::string path, size_t blockSize);
	~SReadStream();

	size_t openFile();
	std::shared_ptr<SBlock> readNextBlock();
	bool isFileEnd() const;
	
private:
	std::ifstream mFileReader;
	std::string   mFilePath;
	size_t        mBlockSize;
	size_t        mFileSize;
	uint32        mIndex;
};
#endif //sign_READ_STREAM