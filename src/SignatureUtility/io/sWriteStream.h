#ifndef sign_WRITE_STREAM
#define sign_WRITE_STREAM

#include  <fstream>
#include <memory>
#include <string>
#include "sBlock.h"

class SWriteStream
{
public:
	SWriteStream(std::string path);
	~SWriteStream();

	bool openFile();
	void writeToFile(std::string hash);
private:
	std::ofstream mFileWriter;
	std::string   mFilePath;
};
#endif //sign_WRITE_STREAM