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
		printf("\nPartition ###\t\t");
		printf("Size\t\t");
		printf("Offset\t\t");
		printf("Type\t\n");
		printf("-------------\t\t");
		printf("-------------\t");
		printf("-------\t\t");
		printf("-------\t\n");
		for (int i = 0; i < (int) diskInf->pdg->PartitionCount; i++) {
			printf("%d. Partition %d\t", i + 1, diskInf->pdg->PartitionEntry[i].PartitionNumber);
			if (i < 9) printf ("\t");
			printf("%.1f MB\t", diskInf->pdg->PartitionEntry[i].PartitionLength.QuadPart / pow(1024.0, 2));
			printf("%.1f MB\t", diskInf->pdg->PartitionEntry[i].StartingOffset.QuadPart / pow(1024.0, 2));
			wprintf (L"%s\n", diskInf->pdg->PartitionEntry[i].Gpt.Name);
		}
	} else {
		printf("Error code %d\n", GetLastError());
		system("pause");
		return 1;
	}

	return 0;
}