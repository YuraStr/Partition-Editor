#include "Menu.h"
#include "PartitionEditorFunctions.h"

int main() {
	DiskInformation diskInf;
	
	GetListPartition(&diskInf);

	system("pause");
	return 0;
}
