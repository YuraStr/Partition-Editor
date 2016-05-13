#include "PartitionEditorFunctions.h"

#include <stdio.h>
#include <math.h>
#include <iostream>

int GetListPartition(DiskInformation *diskInf) {
	DRIVE_LAYOUT_INFORMATION_EX *pdg = (DRIVE_LAYOUT_INFORMATION_EX*)malloc(sizeof(DRIVE_LAYOUT_INFORMATION_EX) + 16 * sizeof(PARTITION_INFORMATION_EX));
	bool bResult;
	int flag = 1;
	bResult = GetDriveLayout(pdg);
	if (bResult) {
		diskInf->pdg = pdg;
		diskInf->partitionEntry = (PartitionInformation*)malloc(pdg->PartitionCount * sizeof(PartitionInformation));
		GetPartitionInformation(diskInf);
		for (int i = 0; i < diskInf->partitionCount; i++) {
			printf("%d. Partition %d\t", i + 1, diskInf->partitionEntry[i].partitionInformation.PartitionNumber);
			printf("%d\t", diskInf->pdg->PartitionStyle);
			if (diskInf->partitionEntry[i].partitionInformation.PartitionStyle == PARTITION_STYLE_RAW) {
				printf("Empty space\t");
			} else {
				printf("%x\t\t", diskInf->partitionEntry[i].partitionInformation.Mbr.PartitionType);
				/*if (diskInf->partitionEntry[i].partitionInformation.PartitionStyle == PARTITION_STYLE_MBR) {
					if (diskInf->partitionEntry[i].partitionInformation.Mbr.PartitionType == 0x07)
						printf("NTFS\t\t");
					if (diskInf->partitionEntry[i].partitionInformation.Mbr.PartitionType == 0x05)
						printf("Extended\t\t");
					if (diskInf->partitionEntry[i].partitionInformation.Mbr.PartitionType == 0x0B)
						printf("FAT32\t\t");
					if (diskInf->partitionEntry[i].partitionInformation.Mbr.PartitionType == 0x04)
						printf("FAT16\t\t");
					if (diskInf->partitionEntry[i].partitionInformation.Mbr.PartitionType == 0x01)
						printf("FAT12\t\t");
					if (diskInf->partitionEntry[i].partitionInformation.Mbr.PartitionType == 0x42)
						printf("LDM\t\t");
					if (diskInf->partitionEntry[i].partitionInformation.Mbr.PartitionType == 0x80)
						printf("NTFT\t\t");
				}*/
			}
			printf("%.1f MB\t", diskInf->partitionEntry[i].partitionInformation.PartitionLength.QuadPart / pow(1024.0, 2));
			printf("%.1f MB\n", diskInf->partitionEntry[i].partitionInformation.StartingOffset.QuadPart / pow(1024.0, 2));
		}
	}
	else {
		printf("Error code %d\n", GetLastError());
		system("pause");
		return 1;
	}
	return 0;
}