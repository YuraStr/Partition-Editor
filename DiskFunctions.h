#include <Windows.h>
#include <ntdddisk.h>

bool GetDriveLayout(DRIVE_LAYOUT_INFORMATION_EX *pgd);
bool GetDriveGeometry(DISK_GEOMETRY *pdg);
