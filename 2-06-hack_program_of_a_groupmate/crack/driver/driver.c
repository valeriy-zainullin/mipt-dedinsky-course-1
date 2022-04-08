#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#pragma GCC diagnostic ignored "-Wsign-compare"
#include <ntddk.h>
#pragma GCC diagnostic pop

NTSTATUS DriverEntry(PDRIVER_OBJECT driver_object, PUNICODE_STRING registry_path) {
	(void) driver_object;
	(void) registry_path;
	
	DbgPrint("%s\n", "Hello ring0!");
	
	return STATUS_SUCCESS;
}

VOID driver_unload(PDRIVER_OBJECT driver_object) {
	(void) driver_object;
	
	DbgPrint("%s\n", "Bye ring0!");
}