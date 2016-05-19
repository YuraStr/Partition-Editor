#ifndef PARTITIONINFORMATION_H
#define PARTITIONINFORMATION_H

#include <windows.h>

struct PartitionStructure {
    int index;
    PARTITION_INFORMATION_EX partitionInformation;
    bool isEmptySpace;
};


#endif // PARTITIONINFORMATION_H
