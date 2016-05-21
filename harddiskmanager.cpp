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
    pgd = (DRIVE_LAYOUT_INFORMATION_EX*)malloc(sizeof(DRIVE_LAYOUT_INFORMATION_EX) + 16 * sizeof(PARTITION_INFORMATION_EX));
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

    CloseHandle(hDevice);
}

void HardDiskManager::createNewPartition(int number, int newSize)
{
    int newIndex = partition[number - 1].index;

    if (newSize > partition[number - 1].partitionInformation.PartitionLength.QuadPart) {
        QMessageBox msg;
        msg.setText("Error: new size too big");
        msg.exec();
        return;
    }

    partition[number - 1].isEmptySpace = false;
    pgd->PartitionEntry[newIndex].PartitionLength.QuadPart = newSize * pow(1024, 2);
    pgd->PartitionEntry[newIndex].RewritePartition = TRUE;
    if (pgd->PartitionStyle == PARTITION_STYLE_GPT) {
        pgd->PartitionEntry[newIndex].PartitionStyle = PARTITION_STYLE_GPT;
        strcpy(partition[number - 1].partitionInformation.Gpt.Name,"Basic data partition");
        setNewGUID(newIndex);
        pgd->PartitionEntry[newIndex].Gpt.Attributes = GPT_ATTRIBUTE_PLATFORM_REQUIRED;
    } else {
        pgd->PartitionEntry[newIndex].PartitionStyle = PARTITION_STYLE_MBR;
        pgd->PartitionEntry[newIndex].Mbr.BootIndicator = FALSE;
        pgd->PartitionEntry[newIndex].Mbr.HiddenSectors = 0;
        pgd->PartitionEntry[newIndex].Mbr.PartitionType = 0x07;
        pgd->PartitionEntry[newIndex].Mbr.RecognizedPartition = TRUE;
    }

    DWORD junk;
    HANDLE hDevice;

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

    DeviceIoControl(hDevice,
                    IOCTL_DISK_SET_DRIVE_LAYOUT_EX,
                    NULL,
                    0,
                    pgd,
                    sizeof(DRIVE_LAYOUT_INFORMATION_EX) + 16 * sizeof(PARTITION_INFORMATION_EX),
                    &junk,
                    (LPOVERLAPPED)NULL);
}

void HardDiskManager::setNewGUID(int index)
{
    pgd->PartitionEntry[index].Gpt.PartitionType.Data1 = 0xEBD0A0A2;
    pgd->PartitionEntry[index].Gpt.PartitionType.Data2 = 0xB9E5;
    pgd->PartitionEntry[index].Gpt.PartitionType.Data3 = 0x4433;
    pgd->PartitionEntry[index].Gpt.PartitionType.Data4[0] = 0x87;
    pgd->PartitionEntry[index].Gpt.PartitionType.Data4[1] = 0xC0;
    pgd->PartitionEntry[index].Gpt.PartitionType.Data4[2] = 0x68;
    pgd->PartitionEntry[index].Gpt.PartitionType.Data4[3] = 0xB6;
    pgd->PartitionEntry[index].Gpt.PartitionType.Data4[4] = 0xB7;
    pgd->PartitionEntry[index].Gpt.PartitionType.Data4[5] = 0x26;
    pgd->PartitionEntry[index].Gpt.PartitionType.Data4[6] = 0x99;
    pgd->PartitionEntry[index].Gpt.PartitionType.Data4[7] = 0x77;

    QUuid uuid = QUuid::createUuid();
    GUID guid = GUID(uuid);
    pgd->PartitionEntry[index].Gpt.PartitionId.Data1 = guid.Data1;
    pgd->PartitionEntry[index].Gpt.PartitionId.Data2 = guid.Data2;
    pgd->PartitionEntry[index].Gpt.PartitionId.Data3 = guid.Data3;
    pgd->PartitionEntry[index].Gpt.PartitionId.Data4[0] = guid.Data4[0];
    pgd->PartitionEntry[index].Gpt.PartitionId.Data4[1] = guid.Data4[1];
    pgd->PartitionEntry[index].Gpt.PartitionId.Data4[2] = guid.Data4[2];
    pgd->PartitionEntry[index].Gpt.PartitionId.Data4[3] = guid.Data4[3];
    pgd->PartitionEntry[index].Gpt.PartitionId.Data4[4] = guid.Data4[4];
    pgd->PartitionEntry[index].Gpt.PartitionId.Data4[5] = guid.Data4[5];
    pgd->PartitionEntry[index].Gpt.PartitionId.Data4[6] = guid.Data4[6];
    pgd->PartitionEntry[index].Gpt.PartitionId.Data4[7] = guid.Data4[7];
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
