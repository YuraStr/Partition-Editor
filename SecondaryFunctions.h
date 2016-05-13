#pragma once
#include "DiskStructure.h"
#include "DiskFunctions.h"

bool GetDiskSize(DiskInformation *diskInf);
int FindAvaliableNumber(DiskInformation *diskInf);
void GetPartitionInformation(DiskInformation *diskInf);