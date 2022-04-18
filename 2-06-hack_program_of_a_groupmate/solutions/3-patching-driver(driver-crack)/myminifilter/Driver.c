/*++

Module Name:

    driver.c

Abstract:

    This file contains the driver entry points and callbacks.

Environment:

    Kernel-mode Driver Framework

--*/

#include <fltkernel.h>
#include <ntddk.h>
#include <wdf.h>
#include <initguid.h>

NTSTATUS
DriverEntry(_In_ PDRIVER_OBJECT DriverObject, _In_ PUNICODE_STRING RegistryPath);
#ifdef ALLOC_PRAGMA
#pragma alloc_text (INIT, DriverEntry)
#endif

PFLT_FILTER FilterHandle = NULL;

FLT_PREOP_CALLBACK_STATUS
MmfPreCreate(PFLT_CALLBACK_DATA Data, PCFLT_RELATED_OBJECTS FltObjects, PVOID* CompletionContext);

const FLT_OPERATION_REGISTRATION Callbacks[] = {
	{IRP_MJ_CREATE, 0, MmfPreCreate, NULL},
	{IRP_MJ_OPERATION_END}
};

NTSTATUS
MmfUnload(_In_ FLT_FILTER_UNLOAD_FLAGS Flags);

const FLT_REGISTRATION FilterRegistration = {
	sizeof(FLT_REGISTRATION),
	FLT_REGISTRATION_VERSION,
	0,
	NULL,
	Callbacks,
	MmfUnload,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL
};

/*
// Source: https://docs.microsoft.com/en-us/windows/win32/procthread/zwqueryinformationprocess
// Those recommendations are for user-mode applications, I suppose. Calling win32 libraries
// from drivers is maybe a bad idea? As drivers can be used recursively due to such calls.
// But this driver obviously not, but still kernel functions are used, so why not
// In our case there is no function that suits us from the win32 libraries.
// ZwQueryInformationProcess is, at least, documented in comparison with EPROCESS
// structure.
enum ProcessInformationClass {
	// Retrieves a pointer to a PEB structure
	// that can be used to determine whether
	// the specified process is being debugged,
	// and a unique value used by the system
	// to identify the specified process.
	// It is best to use the
	// CheckRemoteDebuggerPresent and
	// GetProcessId functions to obtain this
	// information.
	ProcessBasicInformation = 0,
	
	// Retrieves a DWORD_PTR value that is the
	// port number of the debugger for the
	// process. A nonzero value indicates that
	// the process is being run under the
	// control of a ring 3 debugger.
	// It is best to use the
	// CheckRemoteDebuggerPresent or
	// IsDebuggerPresent function.
	ProcessDebugPort = 7,
	
	// Determines whether the process is
	// running in the WOW64 environment
	// (WOW64 is the x86 emulator that allows
	// Win32-based applications to run on
	// 64-bit Windows).
	// It is best to use the IsWow64Process
	// function to obtain this information.
	ProcessWow64Information = 26,
	
	// Retrieves a UNICODE_STRING value containing the name of the image file for the process.
	ProcessImageFileName = 27,
	
	// Retrieves a ULONG value indicating whether the process is considered critical.
	// [!Note] This value can be used starting in Windows XP with SP3. Starting in
	// Windows 8.1, IsProcessCritical should be used instead.
	ProcessBreakOnTermination = 29,
	
	ProcessProtectionInformation = 61
};
*/
// WINAPI is not defined here. It's still not useful for x64.
#if !defined(WINAPI)
#define WINAPI __stdcall
#endif
typedef NTSTATUS(WINAPI* ZwQueryInformationProcessType)(
	_In_      HANDLE           ProcessHandle,
	_In_      PROCESSINFOCLASS ProcessInformationClass,
	_Out_     PVOID            ProcessInformation,
	_In_      ULONG            ProcessInformationLength,
	_Out_opt_ PULONG           ReturnLength
);
static ZwQueryInformationProcessType ZwQueryInformationProcess = NULL;

FLT_PREOP_CALLBACK_STATUS
MmfPreCreate(PFLT_CALLBACK_DATA Data, PCFLT_RELATED_OBJECTS FltObjects, PVOID* CompletionContext) {

	PAGED_CODE();

	UNREFERENCED_PARAMETER(FltObjects);
	UNREFERENCED_PARAMETER(CompletionContext);

	PFLT_FILE_NAME_INFORMATION FileNameInfo = NULL;
	NTSTATUS Status = FltGetFileNameInformation(Data, FLT_FILE_NAME_NORMALIZED | FLT_FILE_NAME_QUERY_DEFAULT, &FileNameInfo);
	
	if (!NT_SUCCESS(Status)) {
		// Failed to check file name. Proceed without filtering.
		return FLT_PREOP_SUCCESS_WITH_CALLBACK;
	}
	
	Status = FltParseFileNameInformation(FileNameInfo);
	
	if (!NT_SUCCESS(Status)) {
		FltReleaseFileNameInformation(FileNameInfo);

		// Failed to check file name. Proceed without filtering.
		return FLT_PREOP_SUCCESS_WITH_CALLBACK;
	}
	
	static const WCHAR TargetFile[] = L"nHack.exe";
	static const WCHAR PatchedFile[] = L"nHack.exe.bin";
	static const WCHAR PatchSuffix[] = L".bin";
	static const SIZE_T PatchSuffixLen = sizeof(PatchSuffix) / sizeof(WCHAR) - 1;
	
	PWSTR FileNameNullTerminated = ExAllocatePool2(POOL_FLAG_NON_PAGED, (FileNameInfo->Name.Length + 1) * sizeof(WCHAR), '3agT');
	if (FileNameNullTerminated == NULL) {
		// Out of memory. Fail the operation.
		// The current policy of the driver is to not
		// let the program see it's patched.
		// So never run program if it's not patched.
		Data->IoStatus.Status = STATUS_NO_MEMORY;
		Data->IoStatus.Information = 0;
		FltReleaseFileNameInformation(FileNameInfo);
		// Don't pass the request to the drivers below in the
		// stack (to other minifilter drivers or to the
		// filesystem).
		// Meaning the driver completed the request with status.
		// Filter manager only calls the post-operation callbacks
		// of minifilter drivers above this driver in the driver
		// stack (docs of Microsoft :)).
		return FLT_PREOP_COMPLETE;
	}
	RtlCopyMemory(FileNameNullTerminated, FileNameInfo->Name.Buffer, FileNameInfo->Name.Length * sizeof(WCHAR));
	
	KdPrint(("Opening file \"%ws\"", FileNameNullTerminated));
	
	if (wcsstr(FileNameNullTerminated, TargetFile) == NULL) {
		// Not our case. Proceed without filtering.
		ExFreePool(FileNameNullTerminated);
		FltReleaseFileNameInformation(FileNameInfo);
		return FLT_PREOP_SUCCESS_WITH_CALLBACK;
	}
	
	// Get the process who generated the request.
	// PEPROCESS Process = IoThreadToProcess(Data->Thread);
	// HANDLE ProcessHandle = PsGetProcessID(Process);
	HANDLE ProcessHandle = PsGetCurrentProcessId();
	
	ULONG ProcessImagePathBufferSize = 0;
	{		
		CHAR EMPTY_BUFFER[1] = {0};
		Status = ZwQueryInformationProcess(ProcessHandle, ProcessImageFileName, EMPTY_BUFFER, 0, &ProcessImagePathBufferSize);
	}
	
	if (NT_SUCCESS(Status)) {
		PUNICODE_STRING ProcessImagePath = ExAllocatePool2(POOL_FLAG_NON_PAGED, ProcessImagePathBufferSize, '4agT');
		if (ProcessImagePath == NULL) {
			// Out of memory. Fail the operation.
			// The current policy of the driver is to not
			// let the program see it's patched.
			Data->IoStatus.Status = STATUS_NO_MEMORY;
			Data->IoStatus.Information = 0;
			ExFreePool(FileNameNullTerminated);
			FltReleaseFileNameInformation(FileNameInfo);
			// Don't pass the request to the drivers below in the
			// stack (to other minifilter drivers or to the
			// filesystem).
			// Meaning the driver completed the request with status.
			// Filter manager only calls the post-operation callbacks
			// of minifilter drivers above this driver in the driver
			// stack (docs of Microsoft :)).
			return FLT_PREOP_COMPLETE;
		}
		Status = ZwQueryInformationProcess(ProcessHandle, ProcessImageFileName, ProcessImagePath, ProcessImagePathBufferSize, NULL);
		if (NT_SUCCESS(Status)) {
			KdPrint(("The requestor is \"%wZ\".\r\n", ProcessImagePath));
				
			UNICODE_STRING TargetFileUnicodeString = {0};
			RtlInitUnicodeString(&TargetFileUnicodeString, TargetFile);

			if (RtlCompareUnicodeString(ProcessImagePath, &TargetFileUnicodeString, TRUE) == 0) {
				// Don't patch for a request from the target app.
				ExFreePool(ProcessImagePath);
				ExFreePool(FileNameNullTerminated);
				FltReleaseFileNameInformation(FileNameInfo);
				// Pass the request down on the driver stack,
				// don't call post-operation callback.
				return FLT_PREOP_SUCCESS_NO_CALLBACK;
			}
		}
		ExFreePool(ProcessImagePath);
	}
	
	if (wcsstr(FileNameNullTerminated, PatchedFile) != NULL) {
		// It's a request for patched file. Skip filtering.
		ExFreePool(FileNameNullTerminated);
		FltReleaseFileNameInformation(FileNameInfo);
		return FLT_PREOP_SUCCESS_WITH_CALLBACK;
	}
	
	SIZE_T NewFileNameSize = FileNameInfo->Name.Length + PatchSuffixLen + 1;
	
	PWSTR NewFileName = ExAllocatePool2(POOL_FLAG_NON_PAGED, NewFileNameSize * sizeof(WCHAR), '5agT');
	
	if (NewFileName == NULL) {
		// Out of memory. Fail the operation.
		// The current policy of the driver is to never
		// allow for the application to run unpatched.
		Data->IoStatus.Status = STATUS_NO_MEMORY;
		Data->IoStatus.Information = 0;
		ExFreePool(FileNameNullTerminated);
		FltReleaseFileNameInformation(FileNameInfo);
		// Don't pass the request to the drivers below in the
		// stack (to other minifilter drivers or to the
		// filesystem).
		// Meaning the driver completed the request with status.
		// Filter manager only calls the post-operation callbacks
		// of minifilter drivers above this driver in the driver
		// stack (docs of Microsoft :)).
		return FLT_PREOP_COMPLETE;
	}
	
	RtlCopyMemory(NewFileName, FileNameInfo->Name.Buffer, FileNameInfo->Name.Length * sizeof(WCHAR));
	
	wcscat(NewFileName, PatchSuffix);

	ASSERT(NewFileNameSize - 1 <= MAXUSHORT);
	
	Status = IoReplaceFileObjectName(Data->Iopb->TargetFileObject, NewFileName, (USHORT) (NewFileNameSize - 1));
	
	if (!NT_SUCCESS(Status)) {
		// Fail the operation.
		Data->IoStatus.Status = Status;
		Data->IoStatus.Information = 0;
		ExFreePool(NewFileName);
		FltReleaseFileNameInformation(FileNameInfo);
		// Complete the request.
		return FLT_PREOP_COMPLETE;
	}
	
	Data->IoStatus.Information = IO_REPARSE;
	Data->IoStatus.Status = STATUS_REPARSE;
	Data->Iopb->TargetFileObject->RelatedFileObject = NULL;

	FltSetCallbackDataDirty(Data);
	
	ExFreePool(NewFileName);
	ExFreePool(FileNameNullTerminated);
	FltReleaseFileNameInformation(FileNameInfo);
	// Return to the I/O operation to the filter manager for further
	// processing and filter manager calls post-operation callback
	// during I/O completion. Whereas with
	// FLT_PREOP_SUCCESS_NO_CALLBACK, post-operation callback is
	// not invoked, if it exists.
	return FLT_PREOP_SUCCESS_WITH_CALLBACK;
}

NTSTATUS
MmfUnload(_In_ FLT_FILTER_UNLOAD_FLAGS Flags) {

	UNREFERENCED_PARAMETER(Flags);

	KdPrint(("myminifiler unload \r\n"));

	FltUnregisterFilter(FilterHandle);
    return STATUS_SUCCESS;
}

NTSTATUS
DriverEntry(_In_ PDRIVER_OBJECT DriverObject, _In_ PUNICODE_STRING RegistryPath) {

	UNREFERENCED_PARAMETER(RegistryPath);

	UNICODE_STRING QueryInfProcName = {0};
	RtlInitUnicodeString(&QueryInfProcName, L"ZwQueryInformationProcess");
	ZwQueryInformationProcess = (ZwQueryInformationProcessType) MmGetSystemRoutineAddress(&QueryInfProcName);
	if (ZwQueryInformationProcess == NULL) {
		return STATUS_UNSUCCESSFUL;
	}

    FltRegisterFilter(DriverObject, &FilterRegistration, &FilterHandle);

    return STATUS_SUCCESS;
}
