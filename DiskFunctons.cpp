#include "DiskFunctions.h"

bool GetDriveLayout(DRIVE_LAYOUT_INFORMATION_EX *pgd) {
	DWORD junk;
	BOOL bResult;
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
	if (hDevice == INVALID_HANDLE_VALUE)
		return false;
	bResult = DeviceIoControl(hDevice,
		IOCTL_DISK_GET_DRIVE_LAYOUT_EX,
		NULL,
		0,
		pgd,
		sizeof(DRIVE_LAYOUT_INFORMATION_EX) + 16 * sizeof(PARTITION_INFORMATION_EX),
		&junk,
		(LPOVERLAPPED)NULL);
	CloseHandle(hDevice);
	return bResult;
}

bool GetDriveGeometry(DISK_GEOMETRY *pdg) {
	DWORD junk;
	BOOL bResult;
	HANDLE hDevice;
	WCHAR diskName[30] = L"\\\\.\\PhysicalDrive0\0";
	hDevice = CreateFile(diskName,
		0,
		FILE_SHARE_READ |
		FILE_SHARE_WRITE,
		NULL,
		OPEN_EXISTING,
		0,
		NULL);
	if (hDevice == INVALID_HANDLE_VALUE) {
		return (FALSE);
	}
	bResult = DeviceIoControl(hDevice,
		IOCTL_DISK_GET_DRIVE_GEOMETRY,
		NULL,
		0,
		pdg,
		sizeof(*pdg),
		&junk,
		(LPOVERLAPPED)NULL);
	CloseHandle(hDevice);
	return (bResult);
}