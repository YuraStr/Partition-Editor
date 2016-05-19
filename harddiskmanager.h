#ifndef HARDDISKMANAGER_H
#define HARDDISKMANAGER_H

#include "partitioninformation.h"

#include <windows.h>
#include <ntdddisk.h>

#include <QMessageBox>

class HardDiskManager
{
private:
    LARGE_INTEGER diskSize;
    int partitionCount;
    PartitionStructure *partition;

    void setDiskSize();
    void fillInPartitionInformation();

public:
    HardDiskManager();
    ~HardDiskManager();

    void createNewPartition();
    void deletePartition();
    void changePartitionSize();

    int getPartitionCount();
    LARGE_INTEGER getDiskSize();
    PartitionStructure getPartition(int index);    
};

#endif // HARDDISKMANAGER_H