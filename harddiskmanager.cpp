#include "harddiskmanager.h"
#define BUFSIZE MAX_PATH
#define FILESYSNAMEBUFSIZE MAX_PATH

HardDiskManager::HardDiskManager()
{
    setDiskSize();
    fillInPartitionInformation();
}

BOOL HardDiskManager::processVolume(HANDLE hVol, char *Buf, int iBufSize)
{
    DWORD serialNumber;
    TCHAR volumeName[MAX_PATH + 1] = { 0 };
    wchar_t FileSysNameBuf[FILESYSNAMEBUFSIZE];
    BOOL bFlag;
    char buffer[20];
    char guid[40];

    GetVolumeInformation((LPCWSTR)Buf,
                         volumeName,
                         BUFSIZE,
                         &serialNumber,
                         0,
                         NULL,
                         (LPWSTR)FileSysNameBuf,
                         FILESYSNAMEBUFSIZE);

    wcstombs(buffer, (wchar_t*)Buf, 20);

    int i, j;
    for (i = 11, j = 0; i < 19; i++, j++) {
        guid[j] = buffer[i];
    }
    guid[j] = '\0';

    char partguid[8];
    for (int i = 0; i < partitionCount; i++) {
        itoa(partition[i].partitionInformation.Gpt.PartitionId.Data1, partguid, 16);
        if (strlen(partguid) < 8) {
            char temp[8];
            char temp2[8];
            for (int j = 0; j < 8 - strlen(partguid); j++)
                temp[j] = '0';
            strcpy(temp2, partguid);
            strcpy(partguid, temp);
            strcat(partguid, temp2);
        }
        if (!strcmp(guid, partguid)) {
            wcstombs(partition[i].file_system, FileSysNameBuf, wcslen(FileSysNameBuf) + 1);
            if (wcscmp(volumeName, L""))
                wcstombs(partition[i].name, volumeName, wcslen(volumeName) + 1);
            partition[i].serial_number = serialNumber;
            break;
        }
    }

    bFlag = FindNextVolume(hVol, (LPWSTR)Buf, iBufSize);

    return (bFlag);
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
            strcpy(partition[number].name, "Local volume");
            strcpy(partition[number].file_system, "Unknown");
            partition[number].isEmptySpace = false;
            partition[number].partitionInformation = pgd->PartitionEntry[i];
            partition[number].partitionInformation.PartitionNumber = number + 1;
            continue;
        }

        if (pgd->PartitionEntry[i].PartitionNumber == 0) {
            if (pgd->PartitionEntry[i - 1].PartitionNumber != 0) {
                ++number;
                partition[number].index = i;
                strcpy(partition[number].name, "Local volume");
                strcpy(partition[number].file_system, "Unknown");
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
            partition[number].free_space.QuadPart = partition[number].partitionInformation.PartitionLength.QuadPart;
        }
    }

    partitionCount = number + 1;

    CloseHandle(hDevice);

    if (partition->partitionInformation.PartitionStyle == PARTITION_STYLE_GPT) {
        char buf[BUFSIZE];
        HANDLE hVol;
        BOOL bFlag;

        hVol = FindFirstVolume((LPWSTR)buf, BUFSIZE);

        if(hVol == INVALID_HANDLE_VALUE) {
            printf("No volumes found!\n");
            return;
        }
        bFlag = processVolume(hVol, buf, BUFSIZE);
        while(bFlag) {
            bFlag = processVolume(hVol, buf, BUFSIZE);
        }

        bFlag = FindVolumeClose(hVol);
    } else {
        for (int i = 0; i < partitionCount; i++) {
            switch (partition[i].partitionInformation.Mbr.PartitionType) {
            case 0x07:
                strcpy(partition[i].file_system, "NTFS");
                break;
            case 0x04:
                strcpy(partition[i].file_system, "FAT16");
                break;
            case 0x0B:
                strcpy(partition[i].file_system, "FAT32");
                break;
            case 0x01:
                strcpy(partition[i].file_system, "FAT12");
                break;
            default:
                break;
            }

        }
    }

    getFreeSpaces();
}

void HardDiskManager::createNewPartition(int number, int newSize)
{
    int newIndex = partition[number - 1].index;

    QMessageBox msg;
    msg.setText(QString::number(newIndex));
    msg.exec();

    if (newSize > partition[number - 1].partitionInformation.PartitionLength.QuadPart) {
        QMessageBox msg;
        msg.setText("Error: new size too big");
        msg.exec();
        return;
    }

    pgd->PartitionEntry[newIndex].PartitionLength.QuadPart = newSize * pow(1024, 2);
    pgd->PartitionEntry[newIndex].StartingOffset = partition[number - 1].partitionInformation.StartingOffset;
    pgd->PartitionEntry[newIndex].RewritePartition = TRUE;
    if (pgd->PartitionStyle == PARTITION_STYLE_GPT) {
        pgd->PartitionEntry[newIndex].PartitionStyle = PARTITION_STYLE_GPT;
        wcscpy(partition[number - 1].partitionInformation.Gpt.Name, L"Basic data partition");
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

    WCHAR diskName[30] = L"\\\\.\\PhysicalDrive0\0";
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
        QMessageBox msg;
        msg.setText("Error: cannot open file");
        msg.exec();
        return;
    }

    BOOL res = DeviceIoControl(hDevice,
                               IOCTL_DISK_SET_DRIVE_LAYOUT_EX,
                               pgd,
                               sizeof(DRIVE_LAYOUT_INFORMATION_EX) + 16 * sizeof(PARTITION_INFORMATION_EX),
                               NULL,
                               0,
                               &junk,
                               (LPOVERLAPPED)NULL);
    if (!res) {
        QMessageBox msg;
        msg.setText("Error: cannot create partition: " + GetLastError());
        msg.exec();
        return;
    }

    CloseHandle(hDevice);

    fillInPartitionInformation();
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

void HardDiskManager::deletePartition(int number)
{
    if (partition[number - 1].isEmptySpace) {
        QMessageBox msg;
        msg.setText("Error: this partitiion if empty");
        msg.exec();
        return;
    }

    int newIndex = partition[number - 1].index;
    pgd->PartitionEntry[newIndex].PartitionLength.QuadPart = 0;
    pgd->PartitionEntry[newIndex].StartingOffset.QuadPart = 0;
    pgd->PartitionEntry[newIndex].PartitionStyle = PARTITION_STYLE_RAW;
    pgd->PartitionEntry[newIndex].PartitionNumber = 0;
    pgd->PartitionEntry[newIndex].RewritePartition = FALSE;
    if (pgd->PartitionStyle == PARTITION_STYLE_GPT) {
        pgd->PartitionEntry[newIndex].Gpt.PartitionType.Data1 = 0x00000000;
        pgd->PartitionEntry[newIndex].Gpt.PartitionType.Data2 = 0x0000;
        pgd->PartitionEntry[newIndex].Gpt.PartitionType.Data3 = 0x0000;
        pgd->PartitionEntry[newIndex].Gpt.PartitionType.Data4[0] = 0x00;
        pgd->PartitionEntry[newIndex].Gpt.PartitionType.Data4[1] = 0x00;
        pgd->PartitionEntry[newIndex].Gpt.PartitionType.Data4[2] = 0x00;
        pgd->PartitionEntry[newIndex].Gpt.PartitionType.Data4[3] = 0x00;
        pgd->PartitionEntry[newIndex].Gpt.PartitionType.Data4[4] = 0x00;
        pgd->PartitionEntry[newIndex].Gpt.PartitionType.Data4[5] = 0x00;
        pgd->PartitionEntry[newIndex].Gpt.PartitionType.Data4[6] = 0x00;
        pgd->PartitionEntry[newIndex].Gpt.PartitionType.Data4[7] = 0x00;
        wcscpy(pgd->PartitionEntry[newIndex].Gpt.Name, L"");
    } else {
        pgd->PartitionEntry[newIndex].Mbr.BootIndicator = FALSE;
        pgd->PartitionEntry[newIndex].Mbr.PartitionType = PARTITION_STYLE_RAW;
        pgd->PartitionEntry[newIndex].Mbr.HiddenSectors = 0;
        pgd->PartitionEntry[newIndex].Mbr.RecognizedPartition = FALSE;
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

    fillInPartitionInformation();
}

void HardDiskManager::changePartitionSize(int number, int size)
{
    DISK_GROW_PARTITION grow;

    grow.PartitionNumber = partition[number - 1].index;
    grow.BytesToGrow.QuadPart = size * pow(1024, 2);

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
                    IOCTL_DISK_GROW_PARTITION,
                    NULL,
                    0,
                    &grow,
                    sizeof(grow),
                    &junk,
                    (LPOVERLAPPED)NULL);

    fillInPartitionInformation();
}

void HardDiskManager::getFreeSpaces()
{
    DWORD serialNumber;

    ULARGE_INTEGER FreeBytesAvailable;
    ULARGE_INTEGER TotalNumberOfBytes;
    ULARGE_INTEGER TotalNumberOfFreeBytes;

    int n;
    char dd[4];
    DWORD dr = GetLogicalDrives();

    for( int i = 0; i < 26; i++ )
    {
        n = ((dr>>i)&0x00000001);
        if( n == 1 ) {
            dd[0] =  char(65+i); dd[1] = ':'; dd[2] = '\\'; dd[3] = 0;

            wchar_t temp[20];
            LPCWSTR str;
            mbstowcs(temp, dd, strlen(dd) + 1);
            str = temp;

            GetDiskFreeSpaceEx(
                str,
                &FreeBytesAvailable,
                &TotalNumberOfBytes,
                &TotalNumberOfFreeBytes
            );

            GetVolumeInformation(str,
                                 NULL,
                                 0,
                                 &serialNumber,
                                 0,
                                 NULL,
                                 NULL,
                                 0);
            for (int j = 0; j < partitionCount; j++) {
                if (partition[j].serial_number == serialNumber && partition[j].isEmptySpace == false) {
                    partition[j].free_space = TotalNumberOfFreeBytes;
                }
            }
        }
    }
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
