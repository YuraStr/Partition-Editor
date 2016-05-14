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
			if (i < 9) printf("\t");
			printf("%.1f MB\t", diskInf->partitionEntry[i].partitionInformation.PartitionLength.QuadPart / pow(1024.0, 2));
			printf("%.1f MB\t", diskInf->partitionEntry[i].partitionInformation.StartingOffset.QuadPart / pow(1024.0, 2));
			if (diskInf->partitionEntry[i].partitionInformation.PartitionStyle == PARTITION_STYLE_RAW) {
				printf("Empty space\t");
			} else {
				if (diskInf->partitionEntry[i].partitionInformation.PartitionStyle == PARTITION_STYLE_MBR) {
					if (diskInf->partitionEntry[i].partitionInformation.Mbr.PartitionType == 0x07)
						printf("NTFS\t");
					if (diskInf->partitionEntry[i].partitionInformation.Mbr.PartitionType == 0x05)
						printf("Extended\t");
					if (diskInf->partitionEntry[i].partitionInformation.Mbr.PartitionType == 0x0B)
						printf("FAT32\t");
					if (diskInf->partitionEntry[i].partitionInformation.Mbr.PartitionType == 0x04)
						printf("FAT16\t");
					if (diskInf->partitionEntry[i].partitionInformation.Mbr.PartitionType == 0x01)
						printf("FAT12\t");
					if (diskInf->partitionEntry[i].partitionInformation.Mbr.PartitionType == 0x42)
						printf("LDM\t");
					if (diskInf->partitionEntry[i].partitionInformation.Mbr.PartitionType == 0x80)
						printf("NTFT\t");
				} else { 
					printf("%x ", diskInf->partitionEntry[i].partitionInformation.Gpt.PartitionType.Data1);
					if (diskInf->partitionEntry[i].partitionInformation.Gpt.PartitionType.Data1 == 0xde94bba4)
						printf("Recovery\t");
					if (diskInf->partitionEntry[i].partitionInformation.Gpt.PartitionType.Data1 == 0xc12a7328)
						printf("EFI System Partition");
					if (diskInf->partitionEntry[i].partitionInformation.Gpt.PartitionType.Data1 == 0xebd0a0a2)
						printf("Basic data Partition");
					if (diskInf->partitionEntry[i].partitionInformation.Gpt.PartitionType.Data1 == 0x657fd6d)
						printf("Linux swap");
					if (diskInf->partitionEntry[i].partitionInformation.Gpt.PartitionType.Data1 == 0xfc63daf)
						printf("Ext 4");
				}
				printf("\n");
			}
		}
	}
	else {
		printf("Error code %d\n", GetLastError());
		system("pause");
		return 1;
	}
	return 0;
}