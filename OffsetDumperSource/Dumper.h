#pragma once
#include <Windows.h>
#include <TlHelp32.h>
#include <stdint.h>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <filesystem>


namespace Dumper
{
	bool ReadProgramIntoMemory(std::string DumpPath, std::vector<uint8_t>* out_buffer);

	bool DumpOffsets(BYTE* Dump, size_t size);
}