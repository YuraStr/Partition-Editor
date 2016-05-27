#ifndef HARDDISKMANAGER_H
#define HARDDISKMANAGER_H

#include "partitionstructure.h"

#include <iostream>
#include <windows.h>
#include <ntdddisk.h>
#include <quuid.h>
#include <qdebug.h>
#include <string.h>
#include <objbase.h>

using namespace std;


#include <QMessageBox>

class HardDiskManager
{
private:
    LARGE_INTEGER diskSize;
    int partitionCount;
    PartitionStructure *partition;
    DRIVE_LAYOUT_INFORMATION_EX *pgd;

    void setDiskSize();
    void fillInPartitionInformation();
    void setNewGUID(int index);
    BOOL processVolume(HANDLE hVol, char *Buf, int iBufSize);

public:
    HardDiskManager();
    ~HardDiskManager();

    void createNewPartition(int number, int size);
    void deletePartition(int number);
    void changePartitionSize(int number, int new_size);

    int getPartitionCount();
    LARGE_INTEGER getDiskSize();
    PartitionStructure getPartition(int index);    
};

#endif // HARDDISKMANAGER_H
