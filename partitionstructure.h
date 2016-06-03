#ifndef PARTITIONSTRUCTURE_H
#define PARTITIONSTRUCTURE_H

#include <windows.h>

struct PartitionStructure {
    int index;
    char name[30];
    char file_system[30];
    ULARGE_INTEGER free_space;
    DWORD serial_number;
    PARTITION_INFORMATION_EX partitionInformation;
    bool isEmptySpace;
};


#endif // PARTITIONSTRUCTURE_H
