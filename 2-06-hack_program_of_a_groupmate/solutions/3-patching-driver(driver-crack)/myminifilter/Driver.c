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

#ifdef ALLOC_PRAGMA
#pragma alloc_text (INIT, DriverEntry)
#endif

NTSTATUS
MmfUnloadCallback(_In_ FLT_FILTER_UNLOAD_FLAGS Flags) {
	KdPrint(("myminifiler unload \r\n"));

	FltUnregisterFilter(FilterHandle);
    return STATUS_SUCCESS;
}

FLT_PREOP_CALLBACK_STATUS
MmfPreCreateCallback(PFLT_CALLBACK_DATA Data, PCFLT_RELATED_OBJECTS FltObjects, PVOID* CompletionContext) {
	PAGED_CODE();

	PFLT_FILE_NAME_INFORMATION FileNameInfo = NULL;
	NTSTATUS Status = FltGetFileNameInformation(Data, FLT_FILE_NAME_NORMALIZED | FLT_FILE_NAME_QUERY_DEFAULT, &FileNameInfo);
	
	if (!NT_SUCCESS(Status)) {
		// Failed to check file name. Proceed without filtering.
		return FLT_PREOP_SUCCESS_WITH_CALLBACK;
	}
	
	Status = FltParseFileNameInformation(FileNameInfo);
	
	if (!NT_SUCESS(Status)) {
		FltReleaseFileNameInformation(FileNameInfo);

		// Failed to check file name. Proceed without filtering.
		return FLT_PREOP_SUCCESS_WITH_CALLBACK;
	}
	
	static const WCHAR TargetFile[] = "nHack.exe";
	static const WCHAR PatchedFile[] = "nHack.exe.bin";
	static const WCHAR PatchSuffix[] = ".bin";
	static const SIZE_T PatchSuffixLen = sizeof(PatchSuffix) / sizeof(WCHAR) - 1;
	
	if (wcsstr(FileNameInfo->Buffer, TargetFile) == NULL) {
		// Not our case. Proceed without filtering.
		FltReleaseFileNameInformation(FileNameInfo);
		return FLT_PREOP_SUCCESS_WITH_CALLBACK;
	}
	
	// Get the process who generated the request.
	HANDLE ProcessHandle = PsGetCurrentProcessId();
	
	ULONG ProcessImagePathBufferSize = 0;
	{
		CHAR EMPTY_BUFFER[1] = {0};
		Status = ZwQueryInformationProcess(ProcessHandle, ProcessImageFileName, EMPTY_BUFFER, 0, &ProcessImageBufferSize);
	}
	
	if (NT_SUCCESS(Status)) {
		PUNICODE_STRING ProcessImagePath = ExAllocatePool2(POOL_FLAG_NON_PAGED, ProcessImagePathBufferSize, '3agT');
		if (ProcessImagePath == NULL) {
			// Out of memory. Fail the operation.
			// The current policy of the driver is to not
			// let the program see it's patched.
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
		Status = ZwQueryInformationProcess(ProcessHandle, ProcessImageFileName, ProcessImagePath, ProcessImagePathBufferSize, NULL);
		if (NT_SUCCESS(Status)) {
			if (wcsstr(ProcessImagePath->Buffer, TargetFile) != NULL) {
				// Don't patch for a request from the target app.
				ExFreePool(ProcessImagePath);
				FltReleaseFileNameInformation(FileNameInfo);
				// Pass the request down on the driver stack,
				// don't call post-operation callback.
				return FLT_PREOP_SUCCESS_NO_CALLBACK;
			}
		}
		ExFreePool(ProcessImagePath);
	}
	
	if (wcsstr(FileNameInfo->Buffer, PatchedFile) != NULL) {
		// It's a request for patched file. Skip filtering.
		FltReleaseFileNameInformation(FileNameInfo);
		return FLT_PREOP_SUCCESS_WITH_CALLBACK;
	}
	
	SIZE_T NewFileNameLen = FileNameInfo->Length + PatchSuffixLen + 1;
	
	PWSTR NewFileName = ExAllocatePool2(POOL_FLAG_NON_PAGED, NewFileNameLen * sizeof(WCHAR), '3agT');
	
	if (NewFileName == NULL) {
		// Out of memory. Fail the operation.
		// The current policy of the driver is to never
		// allow for the application to run unpatched.
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
	
	RtlCopyMemory(NewFileName, FileNameInfo->Buffer, FileNameInfo->Length * sizeof(WCHAR));
	
	wcscat(NewFileName, PatchSuffix);
	
	Status = IoReplaceFileObjectName(Data->Iopb->TargetFileObject, NewFileName, NewFileNameLength)
	
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
	FltReleaseFileNameInformation(FileNameInfo);
	// Return to the I/O operation to the filter manager for further
	// processing and filter manager calls post-operation callback
	// during I/O completion. Whereas with
	// FLT_PREOP_SUCCESS_NO_CALLBACK, post-operation callback is
	// not invoked, if it exists.
	return FLT_PREOP_SUCCESS_WITH_CALLBACK;
}

static const FLT_REGISTRATION = ;

NTSTATUS
DriverEntry(_In_ PDRIVER_OBJECT DriverObject, _In_ PUNICODE_STRING RegistryPath) {

    FLT_REGISTRATION* FltRegistration = ExAllocatePool2(POOL_FLAG_NON_PAGED, sizeof(FLT_REGISTRATION), '1gaT');
    if (FltRegistration == NULL) {
        return STATUS_NO_MEMORY;
    }

    FltRegistration->Size = sizeof(FLT_REGISTRATION);
    FltRegistration->Version = FLT_REGISTRATION_VERSION;
    FltRegistration->FilterUnloadCallback = MmfUnloadCallback;


    FtlRegisterFilter(DriverObject, FltRegistration, );

    return STATUS_SUCCESS;
}
