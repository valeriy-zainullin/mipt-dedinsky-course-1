/*++

Module Name:

    driver.c

Abstract:

    This file contains the driver entry points and callbacks.

Environment:

    Kernel-mode Driver Framework

--*/

// Support for ExAllocatePoolZero.
#define POOL_ZERO_DOWN_LEVEL_SUPPORT

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

	PFLT_FILE_NAME_INFORMATION FileNameInfo = NULL;
	NTSTATUS Status = FltGetFileNameInformation(Data, FLT_FILE_NAME_NORMALIZED | FLT_FILE_NAME_QUERY_DEFAULT | FLT_FILE_NAME_DO_NOT_CACHE, &FileNameInfo);
	
	if (!NT_SUCCESS(Status)) {
		// Failed to check file name. Proceed without filtering.
		return FLT_PREOP_SUCCESS_NO_CALLBACK;
	}
	
	Status = FltParseFileNameInformation(FileNameInfo);
	
	if (!NT_SUCCESS(Status)) {
		FltReleaseFileNameInformation(FileNameInfo);

		// Failed to check file name. Proceed without filtering.
		*CompletionContext = NULL;
		return FLT_PREOP_SUCCESS_NO_CALLBACK;
	}
	
	// DbgPrint("myminifilter: tried to open \"%wZ\".\r\n", FileNameInfo->Name);
		
	static const UNICODE_STRING TargetFile  = RTL_CONSTANT_STRING(L"nHack.exe");
	// Patched file will be "nHack.exe.bin"
	static const WCHAR PatchSuffix[] = L".bin";
	
	if (RtlCompareUnicodeString(&FileNameInfo->FinalComponent, &TargetFile, TRUE) != 0) {
		// Not our case. Proceed without filtering.
		FltReleaseFileNameInformation(FileNameInfo);
		*CompletionContext = NULL;
		return FLT_PREOP_SUCCESS_NO_CALLBACK;
	}
	
	DbgPrint("myminifilter: our case, file is \"%wZ\", final component is \"%wZ\".\r\n", FileNameInfo->Name, FileNameInfo->FinalComponent);

	// Get the process who generated the request.
	PEPROCESS Process = IoThreadToProcess(Data->Thread);
	HANDLE ProcessHandle = NULL;
	Status = ObOpenObjectByPointer(Process, OBJ_KERNEL_HANDLE, NULL, 0, NULL, KernelMode, &ProcessHandle);
	#if DBG
		if (!NT_SUCCESS(Status)) {
			DbgPrint("myminifilter: failed to open a handle to process.\r\n");
			__debugbreak();
		}
	#endif
	
	ULONG ProcessImagePathBufferSize = 0;
	// If opened a handle.
	if (NT_SUCCESS(Status)) {
		CHAR EMPTY_BUFFER[1] = {0};
		Status = ZwQueryInformationProcess(ProcessHandle, ProcessImageFileName, EMPTY_BUFFER, 0, &ProcessImagePathBufferSize);
		DbgPrint("myminifilter: ZwQueryInformationProcess status is 0x%x.\r\n", Status);
		if (Status == STATUS_INFO_LENGTH_MISMATCH) {
			// As expected. We want to get length of the path to the image.
			Status = STATUS_SUCCESS;
		}
		#if DBG
			if (!NT_SUCCESS(Status)) {
				__debugbreak();
			}
		#endif
		
		ZwClose(ProcessHandle);
		ProcessHandle = NULL;
	}
	
	// Don't log failure of ZwQueryInformationProcess. Don't know, maybe some processes don't allow getting their image paths.
	// Check such processes in debug build.
	// Our process is plain user mode, we should be able to do it.
	// If everything previously was successful (if got a handle (otherwise Status is still status from handle acquirement)),
	// opened a handle and got buffer size. 
	if (NT_SUCCESS(Status)) {
		PUNICODE_STRING ProcessImagePath = ExAllocatePoolZero(NonPagedPool, ProcessImagePathBufferSize, '4gaT');
		if (ProcessImagePath == NULL) {
			DbgPrint("myminifiler: out of memory, failed to allocate ProcessImagePath, the request was completed with status STATUS_INSUFFICIENT_RESOURCES.\r\n");
			// Out of memory. Fail the operation.
			// The current policy of the driver is to not
			// let the program see it's patched.
			Data->IoStatus.Status = STATUS_INSUFFICIENT_RESOURCES;
			Data->IoStatus.Information = 0;
			FltReleaseFileNameInformation(FileNameInfo);
			// Don't pass the request to the drivers below in the
			// stack (to other minifilter drivers or to the
			// filesystem).
			// Meaning the driver completed the request with status.
			// Filter manager only calls the post-operation callbacks
			// of minifilter drivers above this driver in the driver
			// stack (docs of Microsoft :)).
			#if DBG
				if (!NT_SUCCESS(Status)) {
					__debugbreak();
				}
			#endif
			return FLT_PREOP_COMPLETE;
		}
		Status = ZwQueryInformationProcess(ProcessHandle, ProcessImageFileName, ProcessImagePath, ProcessImagePathBufferSize, NULL);
		DbgPrint("myminifilter: second ZwQueryInformationProcess status is 0x%x.\r\n", Status);
		if (NT_SUCCESS(Status)) {
			DbgPrint("myminifilter: the requestor is \"%wZ\".\r\n", ProcessImagePath);
				
			// If the process wants to open its own image. And we know that the file being opened is target.
			// Process is target process. No redirection then.
			if (RtlCompareUnicodeString(ProcessImagePath, &FileNameInfo->Name, TRUE) == 0) {
				// Don't patch for a request from the target app.
				ExFreePool(ProcessImagePath);
				FltReleaseFileNameInformation(FileNameInfo);
				// Pass the request down on the driver stack,
				// don't call post-operation callback.
				*CompletionContext = NULL;
				return FLT_PREOP_SUCCESS_NO_CALLBACK;
			}
		}
		#if DBG
		else {
			__debugbreak();
		}
		#endif
		ExFreePool(ProcessImagePath);
	}
	
	static const SIZE_T PatchSuffixLen = sizeof(PatchSuffix) / sizeof(WCHAR) - 1;
	SIZE_T NewFileNameSize = (FileNameInfo->Name.Length / sizeof(WCHAR) + PatchSuffixLen + 1) * sizeof(WCHAR);
	
	PWSTR NewFileName = ExAllocatePoolZero(NonPagedPool, NewFileNameSize, '5agT');
	
	if (NewFileName == NULL) {
		DbgPrint("myminifilter: allocation of \"NewFileName\" failed, request was %zu bytes.\r\n", NewFileNameSize);
		#if DBG
			__debugbreak();
		#endif
		// Out of memory. Fail the operation.
		// The current policy of the driver is to never
		// allow for the application to run unpatched.
		Data->IoStatus.Status = STATUS_INSUFFICIENT_RESOURCES;
		Data->IoStatus.Information = 0;
		FltReleaseFileNameInformation(FileNameInfo);
		// Don't pass the request to the drivers below in the
		// stack (to other minifilter drivers or to the
		// filesystem).
		return FLT_PREOP_COMPLETE;
	}
	
	RtlCopyMemory(NewFileName, FileNameInfo->Name.Buffer, FileNameInfo->Name.Length);
	
	wcscat(NewFileName, PatchSuffix);

	ASSERT(NewFileNameSize - 1 <= MAXUSHORT);
	
	DbgPrint("myminifilter: replacing, Data->Flags is %u, Data->Iopb->IrpFlags is %hhu, Data->Iopb->MajorFunction is %hhu, Data->Iopb->OperationFlags is %hhu.\r\n", Data->Flags, Data->Iopb->IrpFlags, Data->Iopb->MajorFunction, Data->Iopb->OperationFlags);
	
	Status = IoReplaceFileObjectName(Data->Iopb->TargetFileObject, NewFileName, (USHORT) (NewFileNameSize - sizeof(WCHAR)));
	
	if (!NT_SUCCESS(Status)) {
		DbgPrint(
			"myminifiler: failed to replace file object name, "
			"target is \"%wZ\", replacement status is %X.\r\n",
			FileNameInfo->Name,
			Status
		);
		#if DBG
			__debugbreak();
		#endif
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
	// I don't think it's a good idea to touch
	// Data->Iopb->TargetFileObject->RelatedFileObject like this.
	// There may be some reference counting and etc, I suppose.
	// It doesn't really matter what object the file is opened relative to.
	// And it's what the field is meant for from what I understood from the docs.
	// Also this is not done in Microsoft sample called simrep.
	// Data->Iopb->TargetFileObject->RelatedFileObject = NULL;

	// We actually don't need to call this.
	// A minifilter driver's preoperation (PFLT_PRE_OPERATION_CALLBACK) or
	// postoperation (PFLT_POST_OPERATION_CALLBACK) callback routine can
	// modify the contents of the callback data (FLT_CALLBACK_DATA) structure
	// for the operation. If it does, it must then call FltSetCallbackDataDirty
	// unless it has changed the contents of the callback data structure's
	// IoStatus field.
	// FltSetCallbackDataDirty(Data);
	
	ExFreePool(NewFileName);
	FltReleaseFileNameInformation(FileNameInfo);
	// We completed the request with STATUS_REPARSE. This status means that
	// the request resulted in a symbolic link and the object manager
	// should perform a reparse.
	// We haven't found a symbolic link actually, but there are examples
	// of path replacement and it's done like this, first point. And
	// second point is that all drivers and stuff above this driver will
	// see this file as a symbolic link. If we unload the driver, of course,
	// then requests might be cached. But it doesn't matter, it's a showcase
	// driver, we don't unload it.
	return FLT_PREOP_COMPLETE;
}

NTSTATUS
MmfUnload(_In_ FLT_FILTER_UNLOAD_FLAGS Flags) {

	UNREFERENCED_PARAMETER(Flags);

	DbgPrint("myminifiler: unload\r\n");

	FltUnregisterFilter(FilterHandle);
	return STATUS_SUCCESS;
}

NTSTATUS
DriverEntry(_In_ PDRIVER_OBJECT DriverObject, _In_ PUNICODE_STRING RegistryPath) {

	UNREFERENCED_PARAMETER(RegistryPath);
	
	// Support for ExAllocatePoolZero.
	ExInitializeDriverRuntime(DrvRtPoolNxOptIn);
	
	DbgPrint("myminifilter: driver entry\r\n");

	static const UNICODE_STRING QueryInfProcName = RTL_CONSTANT_STRING(L"ZwQueryInformationProcess");
	ZwQueryInformationProcess = (ZwQueryInformationProcessType) MmGetSystemRoutineAddress((PUNICODE_STRING) &QueryInfProcName);
	if (ZwQueryInformationProcess == NULL) {
		DbgPrint("myminifilter: ZwQueryInformationProcess wasn't found.\r\n");
		#if DBG
			__debugbreak();
		#endif
		return STATUS_UNSUCCESSFUL;
	}

	NTSTATUS Status = FltRegisterFilter(DriverObject, &FilterRegistration, &FilterHandle);
	
	if (!NT_SUCCESS(Status)) {
		DbgPrint("myminifilter: failed to register.\r\n");
		#if DBG
			__debugbreak();
		#endif
		return Status;
	}
	
	Status = FltStartFiltering(FilterHandle);
	
	if (!NT_SUCCESS(Status)) {
		DbgPrint("myminifilter: failed to start filtering.\r\n");
		#if DBG
			__debugbreak();
		#endif
		FltUnregisterFilter(FilterHandle);
		return Status;
	}

	return STATUS_SUCCESS;
}
