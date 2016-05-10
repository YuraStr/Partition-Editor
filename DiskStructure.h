#include <Windows.h>

struct DiskInformation {
	LARGE_INTEGER diskSize;
	DRIVE_LAYOUT_INFORMATION_EX *pdg;
};