#include "harddiskmanager.h"

HardDiskManager::HardDiskManager()
{
    setDiskSize();
    fillInPartitionInformation();
}

void HardDiskManager::setDiskSize()
{
    HANDLE hDisk;
    WCHAR diskName[30] = L"\\\\.\\PhysicalDrive0";
    DWORD junk;

    DISK_GEOMETRY *diskGeometry = (DISK_GEOMETRY*) malloc (sizeof(DISK_GEOMETRY));

    hDisk = CreateFile(diskName,
                       0,
                       FILE_SHARE_READ,
                       NULL,
                       OPEN_EXISTING,
                       0,
                       NULL);

    DeviceIoControl(hDisk,
                    IOCTL_DISK_GET_DRIVE_GEOMETRY,
                    NULL,
                    0,
                    diskGeometry,
                    sizeof(*diskGeometry),
                    &junk,
                    (LPOVERLAPPED)NULL);

    diskSize.QuadPart = diskGeometry->Cylinders.QuadPart * diskGeometry->TracksPerCylinder *
                        diskGeometry->SectorsPerTrack    * diskGeometry->BytesPerSector;

    free(diskGeometry);
    CloseHandle(hDisk);
}

void HardDiskManager::fillInPartitionInformation()
{
    DWORD junk;
    HANDLE hDevice;
    DRIVE_LAYOUT_INFORMATION_EX *pgd = (DRIVE_LAYOUT_INFORMATION_EX*)malloc(sizeof(DRIVE_LAYOUT_INFORMATION_EX) +
                                                                                16 * sizeof(PARTITION_INFORMATION_EX));
    WCHAR diskName[30] = L"\\\\.\\PhysicalDrive0";
    hDevice = CreateFile(diskName,
        GENERIC_READ |
        GENERIC_WRITE,
        FILE_SHARE_READ |
        FILE_SHARE_WRITE,
        NULL,
        OPEN_EXISTING,
        0,
        NULL);
    if (hDevice == INVALID_HANDLE_VALUE) {
        partitionCount = -1;
        return;
    } else {
        DeviceIoControl(hDevice,
            IOCTL_DISK_GET_DRIVE_LAYOUT_EX,
            NULL,
            0,
            pgd,
            sizeof(DRIVE_LAYOUT_INFORMATION_EX) + 16 * sizeof(PARTITION_INFORMATION_EX),
            &junk,
       (LPOVERLAPPED)NULL);
    }
    CloseHandle(hDevice);

    int number = -1;
    partition = (PartitionStructure*) malloc (sizeof(PartitionStructure) * (pgd->PartitionCount));
    for (uint i = 0; i < pgd->PartitionCount; i++) {
        if (pgd->PartitionEntry[i].PartitionNumber != 0) {
            ++number;
            partition[number].index = i;
            partition[number].isEmptySpace = false;
            partition[number].partitionInformation = pgd->PartitionEntry[i];
            partition[number].partitionInformation.PartitionNumber = number + 1;
            continue;
        }

        if (pgd->PartitionEntry[i].PartitionNumber == 0) {
            if (pgd->PartitionEntry[i - 1].PartitionNumber != 0) {
                ++number;
                partition[number].index = i;
                partition[number].isEmptySpace = true;
                partition[number].partitionInformation.PartitionStyle = PARTITION_STYLE_RAW;
                partition[number].partitionInformation.PartitionNumber = number + 1;
            }
            if (i != pgd->PartitionCount-1) {
                partition[number].partitionInformation.PartitionLength.QuadPart =
                        partition[i + 1].partitionInformation.StartingOffset.QuadPart -
                        partition[number - 1].partitionInformation.StartingOffset.QuadPart -
                        partition[number - 1].partitionInformation.PartitionLength.QuadPart;
            } else {
                partition[number].partitionInformation.PartitionLength.QuadPart = diskSize.QuadPart -
                        partition[number - 1].partitionInformation.PartitionLength.QuadPart -
                        partition[number - 1].partitionInformation.StartingOffset.QuadPart;
            }
            partition[number].partitionInformation.StartingOffset.QuadPart =
                    partition[number - 1].partitionInformation.PartitionLength.QuadPart +
                    partition[number - 1].partitionInformation.StartingOffset.QuadPart;
        }
    }

    partitionCount = number + 1;

    free(pgd);
}

int HardDiskManager::getPartitionCount()
{
    return partitionCount;
}

LARGE_INTEGER HardDiskManager::getDiskSize()
{
    return diskSize;
}

PartitionStructure HardDiskManager::getPartition(int index)
{
    return partition[index];
}

HardDiskManager::~HardDiskManager()
{
    free(partition);
}
