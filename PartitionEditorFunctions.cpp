#include "PartitionEditorFunctions.h"

#include <stdio.h>
#include <math.h>
#include <iostream>

int GetListPartition(DiskInformation *diskInf) {
	DRIVE_LAYOUT_INFORMATION_EX *pdg = (DRIVE_LAYOUT_INFORMATION_EX*)malloc(sizeof(DRIVE_LAYOUT_INFORMATION_EX) + 16 * sizeof(PARTITION_INFORMATION_EX));
	bool bResult;
	bResult = GetDriveLayout(pdg);
	if (bResult) {
		diskInf->pdg = pdg;
		for (int i = 0; i < (int) diskInf->pdg->PartitionCount; i++) {
			if (diskInf->pdg->PartitionEntry[i].PartitionNumber == i + 1) {
				printf("%d. Partition %d\t", i + 1, diskInf->pdg->PartitionEntry[i].PartitionNumber);
				if (i < 9) printf ("\t");
				if (diskInf->pdg->PartitionEntry[i].PartitionLength.QuadPart == 0 && i != 0) {
					printf("%.1f MB\t", (diskInf->pdg->PartitionEntry[i + 1].StartingOffset.QuadPart - diskInf->pdg->PartitionEntry[i - 1].PartitionLength.QuadPart) / pow(1024, 2));
					printf("%.1f MB\t", (diskInf->pdg->PartitionEntry[i - 1].StartingOffset.QuadPart + diskInf->pdg->PartitionEntry[i - 1].PartitionLength.QuadPart) / pow(1024, 2));
				} else {
					printf("%.1f MB\t", diskInf->pdg->PartitionEntry[i].PartitionLength.QuadPart / pow(1024, 2));
					printf("%.1f MB\t", diskInf->pdg->PartitionEntry[i].StartingOffset.QuadPart / pow(1024, 2));
				}
				if (diskInf->pdg->PartitionStyle == PARTITION_STYLE_GPT) {
					wprintf(L"%s\n", diskInf->pdg->PartitionEntry[i].Gpt.Name);
				} else {
					printf("%d\n", diskInf->pdg->PartitionEntry[i].Mbr.PartitionType);
				}
			}
		}
	} else {
		printf("Error code %d\n", GetLastError());
		system("pause");
		return 1;
	}

	return 0;
}

int CreatePartition(DiskInformation *diskInf) {
	bool bResult;
	int number;
	int length;
	int index;
	int partitionNumber;
	
	printf("\n\nSelect free space to allocate partition\n");
	printf("DISK_MANAGER> ");
	scanf("%d", &number);
	partitionNumber = diskInf->partitionEntry[number - 1].number;
	index = diskInf->partitionEntry[number - 1].index;
	
	printf("\nEnter partition length (MB)\n");
	printf("DISK_MANAGER> ");
	scanf("%d", &length);
	diskInf->pdg->PartitionEntry[index].PartitionLength.QuadPart = length * pow(1024.0, 2);
	diskInf->pdg->PartitionEntry[index].PartitionNumber = partitionNumber;
	diskInf->pdg->PartitionEntry[index].StartingOffset = diskInf->partitionEntry[number - 1].offset;
	diskInf->pdg->PartitionEntry[index].PartitionStyle = PARTITION_STYLE_GPT;
	diskInf->pdg->PartitionEntry[index].RewritePartition = TRUE;
	
	bResult = FALSE;
	bResult = SetDriveLayout(diskInf->pdg);
	if (bResult) {
		printf("\nSuccess\n");
	}
	else {
		printf("Error code %d\n", GetLastError());
		system("pause");
		return ((int)bResult);
	}


	return ((int)bResult);
}

int DeletePartition(DiskInformation *diskInf) {
	bool bResult;
	int number;
	int index;
	int partitionNumber;

	printf("\n\nSelect partition to remove: ");
	partitionNumber = diskInf->partitionEntry[number - 1].number;
	index = diskInf->partitionEntry[number - 1].index;

	diskInf->pdg->PartitionEntry[index].PartitionLength.QuadPart = 0;
	diskInf->pdg->PartitionEntry[index].PartitionNumber = 0;
	diskInf->pdg->PartitionEntry[index].StartingOffset.QuadPart = 0;
	diskInf->pdg->PartitionEntry[index].RewritePartition = FALSE;

	bResult = FALSE;
	bResult = SetDriveLayout(diskInf->pdg);
	if (bResult) {
		printf("\nSuccess\n");
	}
	else {
		printf("Error code %d\n", GetLastError());
		system("pause");
		return ((int)bResult);
	}
	return ((int)bResult);
}
