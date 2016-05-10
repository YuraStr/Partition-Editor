#include "Menu.h"
#include "PartitionEditorFunctions.h"

int main() {
	int choice;
	int flag = 1;
	DiskInformation *diskInf;
	
	GetListPartition(&diskInf);

	system("pause");
	return 0;
}
