#include "Dumper.h"

int main(const int argc, char** argv)
{
	const std::string DumpPath = argv[1];

	printf("[*] Checking Path\n");
	
	if (std::filesystem::exists(DumpPath))
	{
		printf("\t[+] File: %s\n", argv[1]);
	}
	else
	{
		printf("\t[+] File: %s\n", argv[1]);
		std::cin.get();
		return 0;
	}

	printf("[*] Reading File into Memory");

	std::vector<uint8_t> RawFile = { 0 };
	if (!Dumper::ReadProgramIntoMemory(DumpPath, &RawFile))
	{
		printf("\t [-] Failed to read file into memory \n");
		std::cin.get();
		return 0;
	}

	printf("[*] Getting Offsets..\n\n\n");

	Dumper::DumpOffsets(RawFile.data(), RawFile.size());

	std::cin.get();
	return 0;
}