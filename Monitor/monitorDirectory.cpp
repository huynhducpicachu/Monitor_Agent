#include <fltKernel.h>
#include <dontuse.h>
#include <suppress.h>

PFLT_FILTER gFilterHandle = NULL;

NTSTATUS FilterUnload(_In_ FLT_FILTER_UNLOAD_FLAGS Flags) {
    UNREFERENCED_PARAMETER(Flags);

    FltUnregisterFilter(gFilterHandle);
    return STATUS_SUCCESS;
}

FLT_PREOP_CALLBACK_STATUS PreOperationCallback(_Inout_ PFLT_CALLBACK_DATA Data, _In_ PCFLT_RELATED_OBJECTS FltObjects, _Flt_CompletionContext_Outptr_ PVOID* CompletionContext) {
    UNREFERENCED_PARAMETER(FltObjects);
    UNREFERENCED_PARAMETER(CompletionContext);

    // Kiểm tra xem hoạt động là một yêu cầu mở tệp tin
    if (Data->Iopb->MajorFunction == IRP_MJ_CREATE) {
        UNICODE_STRING fileName;
        RtlInitUnicodeString(&fileName, L"your_usb_path_here");

        // Kiểm tra xem tên tệp tin chứa "your_usb_path_here" không
        if (FsRtlIsNameInExpression(&fileName, &Data->Iopb->TargetFileObject->FileName, TRUE, NULL)) {
            DbgPrint("File copied from USB detected: %wZ\n", &Data->Iopb->TargetFileObject->FileName);
            // Thực hiện hành động phản ứng
        }
    }

    return FLT_PREOP_SUCCESS_NO_CALLBACK;
}

NTSTATUS DriverEntry(_In_ PDRIVER_OBJECT DriverObject, _In_ PUNICODE_STRING RegistryPath) {
    UNREFERENCED_PARAMETER(RegistryPath);

    NTSTATUS status = FltRegisterFilter(DriverObject, &FilterRegistration, &gFilterHandle);
    if (NT_SUCCESS(status)) {
        status = FltStartFiltering(gFilterHandle);
        if (!NT_SUCCESS(status)) {
            FltUnregisterFilter(gFilterHandle);
        }
    }

    return status;
}
