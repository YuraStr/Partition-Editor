#pragma once
#include <Windows.h>

struct PartitionInformation {
	int index;
	PARTITION_INFORMATION_EX partitionInformation;
	bool isEmptySpace;
};

struct DiskInformation {
	int partitionCount;
	LARGE_INTEGER diskSize;
	DRIVE_LAYOUT_INFORMATION_EX *pdg;
	PartitionInformation *partitionEntry;
};