#include "Dumper.h"



void Parse(char* combo, char* pattern, char* mask)
{
    char lastChar = ' ';
    unsigned int j = 0;

    for (unsigned int i = 0; i < strlen(combo); i++)
    {
        if ((combo[i] == '?' || combo[i] == '*') && (lastChar != '?' && lastChar != '*'))
        {
            pattern[j] = mask[j] = '?';
            j++;
        }

        else if (isspace(lastChar))
        {
            pattern[j] = lastChar = (char)strtol(&combo[i], 0, 16);
            mask[j] = 'x';
            j++;
        }
        lastChar = combo[i];
    }
    pattern[j] = mask[j] = '\0';
}



char* ScanBasic(char* pattern, char* mask, char* begin, intptr_t size)
{
    intptr_t patternLen = strlen(mask);

    for (int i = 0; i < size; i++)
    {
        bool found = true;
        for (int j = 0; j < patternLen; j++)
        {
            if (mask[j] != '?' && pattern[j] != *(char*)((intptr_t)begin + i + j))
            {
                found = false;
                break;
            }
        }
        if (found)
        {
            return (begin + i);
        }
    }
    return nullptr;
}

//https://guidedhacking.com/threads/universal-pattern-signature-parser.9588/ 
char* Scan(char* combopattern, char* begin, size_t size)
{
    char pattern[100];
    char mask[100];
    Parse(combopattern, pattern, mask);
    return ScanBasic(pattern, mask, begin, size);
}

bool Dumper::ReadProgramIntoMemory(std::string DumpPath, std::vector<uint8_t>* out_buffer)
{
	std::ifstream file_ifstream(DumpPath, std::ios::binary);

	if (!file_ifstream)
		return false;

	out_buffer->assign((std::istreambuf_iterator<char>(file_ifstream)), std::istreambuf_iterator<char>());
	file_ifstream.close();

	return true;
}

bool Dumper::DumpOffsets(BYTE* Dump, size_t size)
{
    printf("Global offsets:\n");

    std::ofstream myfile("Offsets.h");
    if (myfile.is_open())
    {

        myfile << "#include <iostream>\n\n\n";
        myfile << "namespace offsets\n{\n";
    }

    auto cGame = Scan((char*)"48 8B 1D ? ? ? ? 48 89 5C 24 ? 48 85 DB 0F 84 ? ? ? ? 48 8B CB E8 ? ? ? ? 48 85 C0 0F 84 ? ? ? ? 48 8B CB E8 ? ? ? ? 48 8B D8 48 89 84 24 ? ? ? ? 4C 89 B4 24 ? ? ? ? 48 C7 84 24 ? ? ? ? ? ? ? ? 4C 89", (char*)Dump, size);

    if (cGame)
    {
        printf("Found cGame\n");
        ULONG32 FoundRelativeOffset = *(int*)(cGame + 3);
        auto RelativeOffset = ((ULONG64)cGame - (ULONG64)Dump) + 0xA00; //for some reason, addresses are off by 0xA00 bytes i have no clue why, assuming its something to do with vector bullshit or the way i read the program
        ULONG64 Address = (ULONG64)(RelativeOffset + FoundRelativeOffset + 7);
        myfile << std::hex << "constexpr std::ptrdiff_t cGamePtr : 0x" << Address << ";\n";
        printf("\t[+]Offset : 0x%x\n", ((char*)Address));
    }
    else
    {
        printf("Failed to find cGame\n");
    }

    auto SpeedMultiplier = Scan((char*)"F3 0F 10 05 ? ? ? ? F3 0F 59 81 ? ? ? ? C3 48 8D 64 24 ? 48 8B 4C 24 ? E9 ? ? ? ?", (char*)Dump, size);

    if (SpeedMultiplier)
    {
        printf("Found SpeedMultiplier\n");
        ULONG32 FoundRelativeOffset = *(int*)(SpeedMultiplier + 4);
        auto RelativeOffset = ((ULONG64)SpeedMultiplier - (ULONG64)Dump) + 0xA00; //for some reason, addresses are off by 0xA00 bytes i have no clue why, assuming its something to do with vector bullshit or the way i read the program
        ULONG64 Address = (ULONG64)(RelativeOffset + FoundRelativeOffset + 8);
        myfile << std::hex << "constexpr std::ptrdiff_t SpeedMultiplier : 0x" << Address << ";\n";
        printf("\t[+]Offset : 0x%x\n", ((char*)Address));
    }
    else
    {
        printf("Failed to find SpeedMultiplier\n");
    }

    auto FirstObject = Scan((char*)"48 8B 81 ? ? ? ? C3 4A A7 B6 40 01 ? ? ? 48 8B 81 D0 25 ? ?", (char*)Dump, size);

    if (FirstObject)
    {
        printf("Found FirstObject\n");
        auto offset = *(int*)(FirstObject + 3);
        printf("\t[+]Offset : 0x%x\n", offset);
        myfile << std::hex << "constexpr std::ptrdiff_t FirstObject : 0x" << offset << ";\n";

    }
    else
    {
        printf("Failed to find FirstObject\n");
    }

    auto cGraphics = Scan((char*)"48 8B ? ? ? ? ? 49 8B ? E8 ? ? ? ? 48 8B ? EB ? 33 FF B2", (char*)Dump, size);

    if (cGraphics)
    {
        printf("Found cGraphics\n");
        auto offset = *(int*)(cGraphics + 3);
        printf("\t[+]Offset : 0x%x\n", offset);
        myfile << std::hex << "constexpr std::ptrdiff_t cGraphics : 0x" << offset << ";\n";

    }
    else
    {
        printf("Failed to find cGraphics\n");
    }



    printf("\n\ncObject Offsets\n");

    auto PlayerStance = Scan((char*)"89 AE ? ? ? ? 49 8B ? 4C 8D", (char*)Dump, size);

    if (PlayerStance)
    {
        printf("Found PlayerStance\n");
        auto offset = *(int*)(PlayerStance + 2);
        printf("\t[+]Offset : 0x%x\n", offset);
        myfile << std::hex << "constexpr std::ptrdiff_t PlayerStance : 0x" << offset << ";\n";

    }
    else
    {
        printf("Failed to find PlayerStance\n");
    }

    auto PlayerBase = Scan((char*)"48 8B ? ? ? ? ? BA ? ? ? ? 48 8D ? ? ? ? ? 41 0F", (char*)Dump, size);
    if (PlayerBase)
    {
        printf("Found PlayerBase\n");
        auto offset = *(int*)(PlayerBase + 3);
        printf("\t[+]Offset : 0x%x\n", offset);
        myfile << std::hex << "constexpr std::ptrdiff_t PlayerBase : 0x" << offset << ";\n";

    }
    else
    {
        printf("Failed to find PlayerBase\n");
    }

    auto PlayerType = Scan((char*)"3B 91 ? ? ? ? 0F 84 ? ? ? ? 89 91", (char*)Dump, size);
    if (PlayerBase)
    {
        printf("Found PlayerType\n");
        auto offset = *(int*)(PlayerType + 2);
        printf("\t[+]Offset : 0x%x\n", offset);
        myfile << std::hex << "constexpr std::ptrdiff_t PlayerType : 0x" << offset << ";\n";

    }
    else
    {
        printf("Failed to find PlayerType\n");
    }

    auto PlayerTeam = Scan((char*)"8B 81 ? ? ? ? 48 8B ? 89 02", (char*)Dump, size);
    if (PlayerTeam)
    {
        printf("Found PlayerTeam\n");
        auto offset = *(int*)(PlayerTeam + 2);
        printf("\t[+]Offset : 0x%x\n", offset);
        myfile << std::hex << "constexpr std::ptrdiff_t PlayerTeam : 0x" << offset << ";\n";

    }
    else
    {
        printf("Failed to find PlayerTeam\n");
    }

    auto NextObject = Scan((char*)"48 8B ? ? ? ? ? 48 85 ? 75 ? 4C 8B ? ? ? ? ? 49 81 C7", (char*)Dump, size);
    if (NextObject)
    {
        printf("Found NextObject\n");
        auto offset = *(int*)(NextObject + 3);
        printf("\t[+]Offset : 0x%x\n", offset);
        myfile << std::hex << "constexpr std::ptrdiff_t NextObject : 0x" << offset << ";\n";

    }
    else
    {
        printf("Failed to find vec2ViewAngle\n");
    }

    auto vec2ViewAngle = Scan((char*)"F3 44 ? ? ? ? ? ? ? E8 ? ? ? ? 0F 28 ? 41 0F ? ? E8 ? ? ? ? F3 0F ? ? F3 0F ? ? 0F 28", (char*)Dump, size);
    if (vec2ViewAngle)
    {
        printf("Found vec2ViewAngle\n");
        auto offset = *(int*)(vec2ViewAngle + 5);
        printf("\t[+]Offset : 0x%x\n", offset);
        myfile << std::hex << "constexpr std::ptrdiff_t vec2ViewAngle : 0x" << (int)offset << ";\n";
    }
    else
    {
        printf("Failed to find vec2ViewAngle\n");
    }

    auto cActor = Scan((char*)"48 8B ? ? ? ? ? C7 04 24 ? ? ? ? 48 89 ? 48 85 ? 74 ? 48 8B ? ? F0 FF ? ? F0 FF ? 48 8B ? 48 83 C4 ? C3 48 B8", (char*)Dump, size);
    if (cActor)
    {
        printf("Found cActor\n");
        auto offset = *(int*)(cActor + 3);
        printf("\t[+]Offset : 0x%x\n", offset);
        myfile << std::hex << "constexpr std::ptrdiff_t cActor : 0x" << offset << ";\n";

    }
    else
    {
        printf("Failed to find cActor\n");
    }

    auto cSkelaton = Scan((char*)"48 8B ? ? ? ? ? 48 85 ? 74 ? 48 8B ? ? ? ? ? 48 8B ? ? ? ? ? 48 85 ? 74 ? 48 8B ? FF 90", (char*)Dump, size);
    if (cSkelaton)
    {
        printf("Found cSkelaton\n");
        auto offset = *(int*)(cSkelaton + 3);
        printf("\t[+]Offset : 0x%x\n", offset);
        myfile << std::hex << "constexpr std::ptrdiff_t cSkelaton : 0x" << offset << ";\n";

    }
    else
    {
        printf("Failed to find cSkelaton\n");
    }

    auto cSkelatonInfo = Scan((char*)"48 8B ? ? 48 8D ? ? 48 8B ? E8 ? ? ? ? 33 C0", (char*)Dump, size);
    if (cSkelatonInfo)
    {
        printf("Found cSkelatonInfo\n");
        auto offset = *(char*)(cSkelatonInfo + 3);
        printf("\t[+]Offset : 0x%x\n", offset);
        myfile << std::hex << "constexpr std::ptrdiff_t cSkelatonInfo : 0x" << (int)offset << ";\n";

    }
    else
    {
        printf("Failed to find cSkelatonInfo\n");
    }

    auto cBoneInfo = Scan((char*)"48 8B ? ? 48 89 ? ? ? 4C 8B ? ? 48 8D ? ? ? 48 8D", (char*)Dump, size);
    if (cBoneInfo)
    {
        printf("Found cBoneInfo\n");
        auto offset = *(char*)(cBoneInfo + 3);
        printf("\t[+]Offset : 0x%x\n", offset);
        myfile << std::hex << "constexpr std::ptrdiff_t cBoneInfo : 0x" << (int)offset << ";\n";
    }
    else
    {
        printf("Failed to find cBoneInfo\n");
    }

    printf("\n\nGraphics offsets:\n");
    auto cCamera = Scan((char*)"48 8B ? ? 0F 28 ? 0F 57 ? ? ? ? ? 0F 28 ? 0F 57 ? ? ? ? ? C6 44 24 40 ? C6 44 24 38", (char*)Dump, size);
    if (cCamera)
    {
        printf("Found cCamera\n");
        auto offset = *(char*)(cCamera + 0x3);

        printf("\t[+]Offset : 0x%x\n", offset);
        myfile << std::hex << "constexpr std::ptrdiff_t cCamera : 0x" << (int)offset << ";\n";

    }
    else
    {
        printf("Failed to find cCamera\n");
    }

    auto cMatrix = Scan((char*)"4C 8B ? ? 4C 8B ? 48 39", (char*)Dump, size);
    if (cMatrix)
    {
        printf("Found cMatrix\n");
        auto offset = *(char*)(cMatrix + 0x3);

        printf("\t[+]Offset : 0x%x\n", offset);
        myfile << std::hex << "constexpr std::ptrdiff_t cMatrix : 0x" << (int)offset << ";\n";

    }
    else
    {
        printf("Failed to find cMatrix\n");
    }

    auto vec4ViewMatrix = Scan((char*)"0F 28 ? ? ? ? ? 0F 29 ? ? ? ? ? 0F 28 ? ? ? ? ? 0F 29 ? ? ? ? ? 0F 28 ? ? ? ? ? 0F 29 ? ? ? ? ? 0F 28 ? ? ? ? ? 0F 29 ? ? ? ? ? 48 81 C4", (char*)Dump, size);
    if (cMatrix)
    {
        printf("Found vec4ViewMatrix\n");
        auto offset = *(int*)(vec4ViewMatrix + 3);

        printf("\t[+]Offset : 0x%x\n", offset);
        myfile << std::hex << "constexpr std::ptrdiff_t vec4ViewMatrix : 0x" << offset << ";\n";

    }
    else
    {
        printf("Failed to find vec4ViewMatrix\n");
    }

    myfile << "}\n";
    myfile.close();


    return true;
}
