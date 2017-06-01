#include "SProgram.h"
#include "SOptions.h"

//#include <vld.h>

static const size_t DEFAULT_BLOCK_SIZE = 1000000;

static void show_usage(std::string name)
{
	std::cerr << "Usage: " << name << " <option(s)> \n"
		<< "Options:\n"
		<< "\t-h,--help\t\tShow this help message\n"
		<< "\t-i,--input path\t\tPath to an input file\n"
		<< "\t-o,--output path\tPath to an output file\n"
		<< "\t-s,--size\t\tSize of the block (optional, default - 1Mb)\n"
		<< "\t-t,--threads num\tNumber of thread for hash counting (optional, default - 10)\n"
		<< std::endl;
}

int main(int argc, char *argv[], char *envp[])
{
	if (argc < 5)
	{
		show_usage(argv[0]);
		return 1;
	}
	uint32 threadsNum = 10;
	std::string inputPath;
	std::string outputPath;
	size_t blockSize = DEFAULT_BLOCK_SIZE;
	for (int i = 1; i < argc; ++i)
	{
		std::string arg = argv[i];
		if (arg == "-i")
		{
			inputPath = argv[++i];
		}
		else if (arg == "-o")
		{
			outputPath = argv[++i];
		}
		else if (arg == "-s")
		{
			blockSize = atol(argv[++i]);
		}
		else if (arg == "-h")
		{
			show_usage(argv[0]);
			return 1;
		}
		else if (arg == "-t")
		{
			threadsNum = atoi(argv[++i]);
		}
	}
	if(inputPath.empty() || outputPath.empty())
	{
		std::cerr << "Argument error. -i and -o are necessary arguments.\n";
		show_usage(argv[0]);
		return 1;
	}
	std::shared_ptr<SOptions> options = std::make_shared<SOptions>(inputPath, outputPath, blockSize);

	SProgram program(threadsNum);
	program.runWithOptions(options);
	return 1;
}