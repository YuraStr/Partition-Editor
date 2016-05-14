#include "SecondaryFunctions.h"

bool GetDiskSize(DiskInformation *diskInf) {
	DISK_GEOMETRY *pdg = (DISK_GEOMETRY*)malloc(sizeof(DISK_GEOMETRY));
	BOOL bResult;
	bResult = GetDriveGeometry(pdg);
	if (bResult) {
		diskInf->diskSize.QuadPart = pdg->Cylinders.QuadPart * (ULONG)pdg->TracksPerCylinder *
			(ULONG)pdg->SectorsPerTrack * (ULONG)pdg->BytesPerSector;
	}
	free(pdg);
	return bResult;
}

int FindAvaliableNumber(DiskInformation *diskInf) {
	int j = 1;
	for (int i = 0; i < diskInf->partitionCount; i++) {
		if (j == diskInf->partitionEntry[i].partitionInformation.PartitionNumber) {
			j++;
			i = 0;
		}
	}
	return j;
}

void GetPartitionInformation(DiskInformation *diskInf) {
	int flag = 1;
	int j = 0;
	GetDiskSize(diskInf);
	for (int i = 0; i < (int)(diskInf->pdg->PartitionCount); i++) {
		if (flag && diskInf->pdg->PartitionEntry[i].PartitionLength.QuadPart == 0 &&
			diskInf->pdg->PartitionEntry[i].StartingOffset.QuadPart == 0) {
			diskInf->partitionEntry[j].index = i;
			diskInf->partitionEntry[j].partitionInformation.PartitionNumber = 0;
			diskInf->partitionEntry[j].partitionInformation.PartitionStyle = PARTITION_STYLE_RAW;
			diskInf->partitionEntry[j].partitionInformation.StartingOffset.QuadPart = 0;
			diskInf->partitionEntry[j].partitionInformation.PartitionLength.QuadPart = 0;
			diskInf->partitionEntry[j].isEmptySpace = true;
			flag = 0;
		} else {
			if ((!flag) && diskInf->pdg->PartitionEntry[i].PartitionLength.QuadPart == 0 &&
				diskInf->pdg->PartitionEntry[i].StartingOffset.QuadPart == 0) {
					diskInf->partitionEntry[j].partitionInformation.PartitionNumber = 0;
					diskInf->partitionEntry[j].partitionInformation.PartitionStyle = PARTITION_STYLE_RAW;
					diskInf->partitionEntry[j].partitionInformation.StartingOffset.QuadPart = 0;
					diskInf->partitionEntry[j].partitionInformation.PartitionLength.QuadPart = 0;
					diskInf->partitionEntry[j].isEmptySpace = true;
					flag = 0;
			} else {
				if ((!flag) && (diskInf->pdg->PartitionEntry[i].PartitionLength.QuadPart != 0 &&
					diskInf->pdg->PartitionEntry[i].StartingOffset.QuadPart != 0)) {
					j++;
					diskInf->partitionEntry[j].index = i;
					diskInf->partitionEntry[j].partitionInformation.PartitionNumber = diskInf->pdg->PartitionEntry[i].PartitionNumber;
					diskInf->partitionEntry[j].partitionInformation.PartitionStyle = diskInf->pdg->PartitionEntry[i].PartitionStyle;
					diskInf->partitionEntry[j].partitionInformation.StartingOffset.QuadPart = diskInf->pdg->PartitionEntry[i].StartingOffset.QuadPart;
					diskInf->partitionEntry[j].partitionInformation.PartitionLength.QuadPart = diskInf->pdg->PartitionEntry[i].PartitionLength.QuadPart;
					diskInf->partitionEntry[j].isEmptySpace = false;
					j++;
					flag = 1;
				} else {
					if (flag && diskInf->pdg->PartitionEntry[i].PartitionLength.QuadPart != 0 &&
						diskInf->pdg->PartitionEntry[i].StartingOffset.QuadPart != 0) {
						diskInf->partitionEntry[j].index = i;diskInf->partitionEntry[j].partitionInformation.PartitionNumber = diskInf->pdg->PartitionEntry[i].PartitionNumber;
						diskInf->partitionEntry[j].partitionInformation.PartitionStyle = diskInf->pdg->PartitionEntry[i].PartitionStyle;
						diskInf->partitionEntry[j].partitionInformation.StartingOffset.QuadPart = diskInf->pdg->PartitionEntry[i].StartingOffset.QuadPart;
						diskInf->partitionEntry[j].partitionInformation.PartitionLength.QuadPart = diskInf->pdg->PartitionEntry[i].PartitionLength.QuadPart;
						diskInf->partitionEntry[j].isEmptySpace = false;
						j++;
					}
				}
			}
		}
		if (diskInf->partitionEntry[j - 1].partitionInformation.PartitionStyle == PARTITION_STYLE_MBR) {
			diskInf->partitionEntry[j - 1].partitionInformation.Mbr = diskInf->pdg->PartitionEntry[i].Mbr;
		}
		if (diskInf->partitionEntry[j - 1].partitionInformation.PartitionStyle == PARTITION_STYLE_GPT) {
			diskInf->partitionEntry[j - 1].partitionInformation.Gpt = diskInf->pdg->PartitionEntry[i].Gpt;
		}
	}
	if (flag) {
		diskInf->partitionCount = j;
	}
	else {
		diskInf->partitionCount = j + 1;
	}
	for (int i = 0; i < diskInf->partitionCount; i++) {
		if (i == 0 && diskInf->partitionEntry[i].isEmptySpace) {
			diskInf->partitionEntry[i].partitionInformation.StartingOffset.QuadPart = 0;
		}
		else
		if (i != 0 && diskInf->partitionEntry[i].isEmptySpace){
			diskInf->partitionEntry[i].partitionInformation.StartingOffset.QuadPart = 
				diskInf->partitionEntry[i - 1].partitionInformation.StartingOffset.QuadPart + 
				diskInf->partitionEntry[i - 1].partitionInformation.PartitionLength.QuadPart;
		}
		if (i == diskInf->partitionCount - 1 && diskInf->partitionEntry[i].isEmptySpace) {
			diskInf->partitionEntry[i].partitionInformation.PartitionLength.QuadPart = 
				diskInf->diskSize.QuadPart - 
				diskInf->partitionEntry[i].partitionInformation.StartingOffset.QuadPart;
		}
		else if (i != diskInf->partitionCount - 1 && diskInf->partitionEntry[i].isEmptySpace){
			diskInf->partitionEntry[i].partitionInformation.PartitionLength.QuadPart = 
				diskInf->partitionEntry[i + 1].partitionInformation.StartingOffset.QuadPart - 
				diskInf->partitionEntry[i].partitionInformation.StartingOffset.QuadPart;
		}
	}

	for (int i = 0; i < diskInf->partitionCount; i++) {
		if (diskInf->partitionEntry[i].isEmptySpace)
			diskInf->partitionEntry[i].partitionInformation.PartitionNumber = FindAvaliableNumber(diskInf);
	}
}