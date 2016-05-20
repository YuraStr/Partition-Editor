#ifndef PARTITIONSTRUCTURE_H
#define PARTITIONSTRUCTURE_H

#include <windows.h>

struct PartitionStructure {
    int index;
    PARTITION_INFORMATION_EX partitionInformation;
    bool isEmptySpace;
};


#endif // PARTITIONSTRUCTURE_H
