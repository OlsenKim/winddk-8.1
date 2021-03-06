/*++

Copyright (c) Microsoft Corporation.  All rights reserved.

_WdfVersionBuild_

Module Name:

    wdfrequest.h

Abstract:

    This module contains contains the Windows Driver Framework Request object
    interfaces.

Environment:

    kernel mode only

Revision History:


--*/

#ifndef _WDFREQUEST_1_9_H_
#define _WDFREQUEST_1_9_H_



#if (NTDDI_VERSION >= NTDDI_WIN2K)

//
// Types
//

typedef enum _WDF_REQUEST_TYPE {
    WdfRequestTypeCreate = 0x0,
    WdfRequestTypeCreateNamedPipe = 0x1,
    WdfRequestTypeClose = 0x2,
    WdfRequestTypeRead = 0x3,
    WdfRequestTypeWrite = 0x4,
    WdfRequestTypeQueryInformation = 0x5,
    WdfRequestTypeSetInformation = 0x6,
    WdfRequestTypeQueryEA = 0x7,
    WdfRequestTypeSetEA = 0x8,
    WdfRequestTypeFlushBuffers = 0x9,
    WdfRequestTypeQueryVolumeInformation = 0xa,
    WdfRequestTypeSetVolumeInformation = 0xb,
    WdfRequestTypeDirectoryControl = 0xc,
    WdfRequestTypeFileSystemControl = 0xd,
    WdfRequestTypeDeviceControl = 0xe,
    WdfRequestTypeDeviceControlInternal = 0xf,
    WdfRequestTypeShutdown = 0x10,
    WdfRequestTypeLockControl = 0x11,
    WdfRequestTypeCleanup = 0x12,
    WdfRequestTypeCreateMailSlot = 0x13,
    WdfRequestTypeQuerySecurity = 0x14,
    WdfRequestTypeSetSecurity = 0x15,
    WdfRequestTypePower = 0x16,
    WdfRequestTypeSystemControl = 0x17,
    WdfRequestTypeDeviceChange = 0x18,
    WdfRequestTypeQueryQuota = 0x19,
    WdfRequestTypeSetQuota = 0x1A,
    WdfRequestTypePnp = 0x1B,
    WdfRequestTypeOther =0x1C,
    WdfRequestTypeUsb = 0x40,
    WdfRequestTypeNoFormat = 0xFF,
    WdfRequestTypeMax,
} WDF_REQUEST_TYPE;

typedef enum _WDF_REQUEST_REUSE_FLAGS {
    WDF_REQUEST_REUSE_NO_FLAGS = 0x00000000,
    WDF_REQUEST_REUSE_SET_NEW_IRP = 0x00000001,
} WDF_REQUEST_REUSE_FLAGS;

// 
// This defines the actions to take on a request
// in EvtIoStop.
// 
typedef enum _WDF_REQUEST_STOP_ACTION_FLAGS {
    WdfRequestStopActionInvalid = 0,
    WdfRequestStopActionSuspend = 0x01, //  Device is being suspended
    WdfRequestStopActionPurge = 0x2, //  Device/queue is being removed
    WdfRequestStopRequestCancelable = 0x10000000, //  This bit is set if the request is marked cancelable
} WDF_REQUEST_STOP_ACTION_FLAGS;

typedef enum _WDF_REQUEST_SEND_OPTIONS_FLAGS {
    WDF_REQUEST_SEND_OPTION_TIMEOUT = 0x0000001,
    WDF_REQUEST_SEND_OPTION_SYNCHRONOUS = 0x0000002,
    WDF_REQUEST_SEND_OPTION_IGNORE_TARGET_STATE = 0x0000004,
    WDF_REQUEST_SEND_OPTION_SEND_AND_FORGET = 0x0000008,
} WDF_REQUEST_SEND_OPTIONS_FLAGS;



// Request cancel is called if a request that has been marked cancelable is cancelled
typedef
_Function_class_(EVT_WDF_REQUEST_CANCEL)
_IRQL_requires_same_
_IRQL_requires_max_(DISPATCH_LEVEL)
VOID
EVT_WDF_REQUEST_CANCEL(
    _In_
    WDFREQUEST Request
    );

typedef EVT_WDF_REQUEST_CANCEL *PFN_WDF_REQUEST_CANCEL;


//
// This parameters structure allows general access to a requests parameters
//
typedef struct _WDF_REQUEST_PARAMETERS {

    USHORT Size;

    UCHAR MinorFunction;

    WDF_REQUEST_TYPE Type;

    //
    // The following user parameters are based on the service that is being
    // invoked.  Drivers and file systems can determine which set to use based
    // on the above major and minor function codes.
    //
    union {

        //
        // System service parameters for:  Create
        //

        struct {
            PIO_SECURITY_CONTEXT SecurityContext;
            ULONG Options;
            USHORT POINTER_ALIGNMENT FileAttributes;
            USHORT ShareAccess;
            ULONG POINTER_ALIGNMENT EaLength;
        } Create;


        //
        // System service parameters for:  Read
        //

        struct {
            size_t Length;
            ULONG POINTER_ALIGNMENT Key;
            LONGLONG DeviceOffset;
        } Read;

        //
        // System service parameters for:  Write
        //

        struct {
            size_t Length;
            ULONG POINTER_ALIGNMENT Key;
            LONGLONG DeviceOffset;
        } Write;

        //
        // System service parameters for:  Device Control
        //
        // Note that the user's output buffer is stored in the UserBuffer field
        // and the user's input buffer is stored in the SystemBuffer field.
        //

        struct {
            size_t OutputBufferLength;
            size_t POINTER_ALIGNMENT InputBufferLength;
            ULONG POINTER_ALIGNMENT IoControlCode;
            PVOID Type3InputBuffer;
        } DeviceIoControl;

        struct {
            PVOID Arg1;
            PVOID  Arg2;
            ULONG POINTER_ALIGNMENT IoControlCode;
            PVOID Arg4;
        } Others;

    } Parameters;

} WDF_REQUEST_PARAMETERS, *PWDF_REQUEST_PARAMETERS;

VOID
FORCEINLINE
WDF_REQUEST_PARAMETERS_INIT(
    _Out_ PWDF_REQUEST_PARAMETERS Parameters
    )
{
    RtlZeroMemory(Parameters, sizeof(WDF_REQUEST_PARAMETERS));

    Parameters->Size = sizeof(WDF_REQUEST_PARAMETERS);
}

typedef struct _WDF_USB_REQUEST_COMPLETION_PARAMS *PWDF_USB_REQUEST_COMPLETION_PARAMS;

typedef struct _WDF_REQUEST_COMPLETION_PARAMS {
    //
    // Size of the structure in bytes
    //
    ULONG Size;

    WDF_REQUEST_TYPE Type;

    IO_STATUS_BLOCK IoStatus;

    union {
        struct {
            WDFMEMORY Buffer;
            size_t Length;
            size_t Offset;
        } Write;

        struct {
            WDFMEMORY Buffer;
            size_t Length;
            size_t Offset;
        } Read;

        struct {
            ULONG IoControlCode;

            struct {
                WDFMEMORY Buffer;
                size_t Offset;
            } Input;

            struct {
                WDFMEMORY Buffer;
                size_t Offset;
                size_t Length;
            } Output;
        } Ioctl;

        struct {
            union {
                PVOID Ptr;
                ULONG_PTR Value;
            } Argument1;
            union {
                PVOID Ptr;
                ULONG_PTR Value;
            } Argument2;
            union {
                PVOID Ptr;
                ULONG_PTR Value;
            } Argument3;
            union {
                PVOID Ptr;
                ULONG_PTR Value;
            } Argument4;
        } Others;

        struct {
            PWDF_USB_REQUEST_COMPLETION_PARAMS Completion;
        } Usb;
    } Parameters;

} WDF_REQUEST_COMPLETION_PARAMS, *PWDF_REQUEST_COMPLETION_PARAMS;

VOID
FORCEINLINE
WDF_REQUEST_COMPLETION_PARAMS_INIT(
    _Out_ PWDF_REQUEST_COMPLETION_PARAMS Params
    )
{
    RtlZeroMemory(Params, sizeof(WDF_REQUEST_COMPLETION_PARAMS));
    Params->Size = sizeof(WDF_REQUEST_COMPLETION_PARAMS);
    Params->Type = WdfRequestTypeNoFormat;
}

typedef
_Function_class_(EVT_WDF_REQUEST_COMPLETION_ROUTINE)
_IRQL_requires_same_
VOID
EVT_WDF_REQUEST_COMPLETION_ROUTINE(
    _In_
    WDFREQUEST Request,
    _In_
    WDFIOTARGET Target,
    _In_
    PWDF_REQUEST_COMPLETION_PARAMS Params,
    _In_
    WDFCONTEXT Context
    );

typedef EVT_WDF_REQUEST_COMPLETION_ROUTINE *PFN_WDF_REQUEST_COMPLETION_ROUTINE;

/*++

Routine Description:
    Clears out the internal state of the irp, which includes, but is not limited
    to:
    a)  Any internal allocations for the previously formatted request
    b)  The completion routine and its context
    c)  The request's intended i/o target
    d)  All of the internal IRP's stack locations

Arguments:
    Request - The request to be reused.

    ReuseParams - Parameters controlling the reuse of the request, see comments
        for each field in the structure for usage

Return Value:
    None

  --*/

typedef struct _WDF_REQUEST_REUSE_PARAMS {
    //
    // Size of this structure in bytes
    //
    ULONG Size;

    //
    // Bit field combination of WDF_REQUEST_REUSE_Xxx values
    //
    ULONG Flags;

    //
    // The new status of the request.
    //
    NTSTATUS Status;

    //
    // New PIRP  to be contained in the WDFREQUEST.   Setting a new PIRP value
    // is only valid for WDFREQUESTs created by WdfRequestCreateFromIrp where
    // RequestFreesIrp == FALSE.  No other WDFREQUESTs (presented by the
    // I/O queue for instance) may have their IRPs changed.
    //
    PIRP NewIrp;

} WDF_REQUEST_REUSE_PARAMS, *PWDF_REQUEST_REUSE_PARAMS;

VOID
FORCEINLINE
WDF_REQUEST_REUSE_PARAMS_INIT(
    _Out_ PWDF_REQUEST_REUSE_PARAMS Params,
    _In_ ULONG Flags,
    _In_ NTSTATUS Status
    )
{
    RtlZeroMemory(Params, sizeof(WDF_REQUEST_REUSE_PARAMS));

    Params->Size = sizeof(WDF_REQUEST_REUSE_PARAMS);
    Params->Flags = Flags;
    Params->Status = Status;
}

VOID
FORCEINLINE
WDF_REQUEST_REUSE_PARAMS_SET_NEW_IRP(
    _Inout_ PWDF_REQUEST_REUSE_PARAMS Params,
    _In_ PIRP NewIrp
    )
{
    Params->Flags |= WDF_REQUEST_REUSE_SET_NEW_IRP;
    Params->NewIrp = NewIrp;
}

typedef struct _WDF_REQUEST_SEND_OPTIONS {
    //
    // Size of the structure in bytes
    //
    ULONG Size;

    //
    // Bit field combination of values from the WDF_REQUEST_SEND_OPTIONS_FLAGS
    // enumeration
    //
    ULONG Flags;

    //
    // Valid when WDF_REQUEST_SEND_OPTION_TIMEOUT is set
    //
    LONGLONG Timeout;

} WDF_REQUEST_SEND_OPTIONS, *PWDF_REQUEST_SEND_OPTIONS;

VOID
FORCEINLINE
WDF_REQUEST_SEND_OPTIONS_INIT(
    _Out_ PWDF_REQUEST_SEND_OPTIONS Options,
    _In_ ULONG Flags
    )
{
    RtlZeroMemory(Options, sizeof(WDF_REQUEST_SEND_OPTIONS));
    Options->Size = sizeof(WDF_REQUEST_SEND_OPTIONS);
    Options->Flags = Flags;
}

VOID
FORCEINLINE
WDF_REQUEST_SEND_OPTIONS_SET_TIMEOUT(
    _Inout_ PWDF_REQUEST_SEND_OPTIONS Options,
    _In_ LONGLONG Timeout
    )
{
    Options->Flags |= WDF_REQUEST_SEND_OPTION_TIMEOUT;
    Options->Timeout = Timeout;
}

typedef enum _WDF_REQUEST_FORWARD_OPTIONS_FLAGS {
    WDF_REQUEST_FORWARD_OPTION_SEND_AND_FORGET = 0x0000001
} WDF_REQUEST_FORWARD_OPTIONS_FLAGS;

typedef struct _WDF_REQUEST_FORWARD_OPTIONS {
    //
    // Size of the structure in bytes
    //
    ULONG Size;

    //
    // Bit field combination of values from the WDF_REQUEST_FORWARD_OPTIONS_FLAGS
    // enumeration
    //
    ULONG Flags;  
} WDF_REQUEST_FORWARD_OPTIONS, *PWDF_REQUEST_FORWARD_OPTIONS;


//
// Default REquest forward initialization macro
//
VOID
FORCEINLINE
WDF_REQUEST_FORWARD_OPTIONS_INIT(
    _Out_ PWDF_REQUEST_FORWARD_OPTIONS  ForwardOptions
    )
{
    RtlZeroMemory(ForwardOptions, sizeof(WDF_REQUEST_FORWARD_OPTIONS));

    ForwardOptions->Size = sizeof(WDF_REQUEST_FORWARD_OPTIONS);
    ForwardOptions->Flags |= WDF_REQUEST_FORWARD_OPTION_SEND_AND_FORGET;
}


//
// WDF Function: WdfRequestCreate
//
typedef
_Must_inspect_result_
_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
NTSTATUS
(*PFN_WDFREQUESTCREATE)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_opt_
    PWDF_OBJECT_ATTRIBUTES RequestAttributes,
    _In_opt_
    WDFIOTARGET IoTarget,
    _Out_
    WDFREQUEST* Request
    );

_Must_inspect_result_
_IRQL_requires_max_(DISPATCH_LEVEL)
NTSTATUS
FORCEINLINE
WdfRequestCreate(
    _In_opt_
    PWDF_OBJECT_ATTRIBUTES RequestAttributes,
    _In_opt_
    WDFIOTARGET IoTarget,
    _Out_
    WDFREQUEST* Request
    )
{
    return ((PFN_WDFREQUESTCREATE) WdfFunctions[WdfRequestCreateTableIndex])(WdfDriverGlobals, RequestAttributes, IoTarget, Request);
}

//
// WDF Function: WdfRequestCreateFromIrp
//
typedef
_Must_inspect_result_
_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
NTSTATUS
(*PFN_WDFREQUESTCREATEFROMIRP)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_opt_
    PWDF_OBJECT_ATTRIBUTES RequestAttributes,
    _In_
    PIRP Irp,
    _In_
    BOOLEAN RequestFreesIrp,
    _Out_
    WDFREQUEST* Request
    );

_Must_inspect_result_
_IRQL_requires_max_(DISPATCH_LEVEL)
NTSTATUS
FORCEINLINE
WdfRequestCreateFromIrp(
    _In_opt_
    PWDF_OBJECT_ATTRIBUTES RequestAttributes,
    _In_
    PIRP Irp,
    _In_
    BOOLEAN RequestFreesIrp,
    _Out_
    WDFREQUEST* Request
    )
{
    return ((PFN_WDFREQUESTCREATEFROMIRP) WdfFunctions[WdfRequestCreateFromIrpTableIndex])(WdfDriverGlobals, RequestAttributes, Irp, RequestFreesIrp, Request);
}

//
// WDF Function: WdfRequestReuse
//
typedef
_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
NTSTATUS
(*PFN_WDFREQUESTREUSE)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFREQUEST Request,
    _In_
    PWDF_REQUEST_REUSE_PARAMS ReuseParams
    );

_IRQL_requires_max_(DISPATCH_LEVEL)
NTSTATUS
FORCEINLINE
WdfRequestReuse(
    _In_
    WDFREQUEST Request,
    _In_
    PWDF_REQUEST_REUSE_PARAMS ReuseParams
    )
{
    return ((PFN_WDFREQUESTREUSE) WdfFunctions[WdfRequestReuseTableIndex])(WdfDriverGlobals, Request, ReuseParams);
}

//
// WDF Function: WdfRequestChangeTarget
//
typedef
_Must_inspect_result_
_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
NTSTATUS
(*PFN_WDFREQUESTCHANGETARGET)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFREQUEST Request,
    _In_
    WDFIOTARGET IoTarget
    );

_Must_inspect_result_
_IRQL_requires_max_(DISPATCH_LEVEL)
NTSTATUS
FORCEINLINE
WdfRequestChangeTarget(
    _In_
    WDFREQUEST Request,
    _In_
    WDFIOTARGET IoTarget
    )
{
    return ((PFN_WDFREQUESTCHANGETARGET) WdfFunctions[WdfRequestChangeTargetTableIndex])(WdfDriverGlobals, Request, IoTarget);
}

//
// WDF Function: WdfRequestFormatRequestUsingCurrentType
//
typedef
_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
VOID
(*PFN_WDFREQUESTFORMATREQUESTUSINGCURRENTTYPE)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFREQUEST Request
    );

_IRQL_requires_max_(DISPATCH_LEVEL)
VOID
FORCEINLINE
WdfRequestFormatRequestUsingCurrentType(
    _In_
    WDFREQUEST Request
    )
{
    ((PFN_WDFREQUESTFORMATREQUESTUSINGCURRENTTYPE) WdfFunctions[WdfRequestFormatRequestUsingCurrentTypeTableIndex])(WdfDriverGlobals, Request);
}

//
// WDF Function: WdfRequestWdmFormatUsingStackLocation
//
typedef
_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
VOID
(*PFN_WDFREQUESTWDMFORMATUSINGSTACKLOCATION)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFREQUEST Request,
    _In_
    PIO_STACK_LOCATION Stack
    );

_IRQL_requires_max_(DISPATCH_LEVEL)
VOID
FORCEINLINE
WdfRequestWdmFormatUsingStackLocation(
    _In_
    WDFREQUEST Request,
    _In_
    PIO_STACK_LOCATION Stack
    )
{
    ((PFN_WDFREQUESTWDMFORMATUSINGSTACKLOCATION) WdfFunctions[WdfRequestWdmFormatUsingStackLocationTableIndex])(WdfDriverGlobals, Request, Stack);
}

//
// WDF Function: WdfRequestSend
//
typedef
_Must_inspect_result_
_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
BOOLEAN
(*PFN_WDFREQUESTSEND)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFREQUEST Request,
    _In_
    WDFIOTARGET Target,
    _In_opt_
    PWDF_REQUEST_SEND_OPTIONS Options
    );

_Must_inspect_result_
_IRQL_requires_max_(DISPATCH_LEVEL)
BOOLEAN
FORCEINLINE
WdfRequestSend(
    _In_
    WDFREQUEST Request,
    _In_
    WDFIOTARGET Target,
    _In_opt_
    PWDF_REQUEST_SEND_OPTIONS Options
    )
{
    return ((PFN_WDFREQUESTSEND) WdfFunctions[WdfRequestSendTableIndex])(WdfDriverGlobals, Request, Target, Options);
}

//
// WDF Function: WdfRequestGetStatus
//
typedef
_Must_inspect_result_
_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
NTSTATUS
(*PFN_WDFREQUESTGETSTATUS)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFREQUEST Request
    );

_Must_inspect_result_
_IRQL_requires_max_(DISPATCH_LEVEL)
NTSTATUS
FORCEINLINE
WdfRequestGetStatus(
    _In_
    WDFREQUEST Request
    )
{
    return ((PFN_WDFREQUESTGETSTATUS) WdfFunctions[WdfRequestGetStatusTableIndex])(WdfDriverGlobals, Request);
}

//
// WDF Function: WdfRequestMarkCancelable
//
typedef
_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
VOID
(*PFN_WDFREQUESTMARKCANCELABLE)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFREQUEST Request,
    _In_
    PFN_WDF_REQUEST_CANCEL EvtRequestCancel
    );

_IRQL_requires_max_(DISPATCH_LEVEL)
VOID
FORCEINLINE
WdfRequestMarkCancelable(
    _In_
    WDFREQUEST Request,
    _In_
    PFN_WDF_REQUEST_CANCEL EvtRequestCancel
    )
{
    ((PFN_WDFREQUESTMARKCANCELABLE) WdfFunctions[WdfRequestMarkCancelableTableIndex])(WdfDriverGlobals, Request, EvtRequestCancel);
}

//
// WDF Function: WdfRequestMarkCancelableEx
//
typedef
_Must_inspect_result_
_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
NTSTATUS
(*PFN_WDFREQUESTMARKCANCELABLEEX)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFREQUEST Request,
    _In_
    PFN_WDF_REQUEST_CANCEL EvtRequestCancel
    );

_Must_inspect_result_
_IRQL_requires_max_(DISPATCH_LEVEL)
NTSTATUS
FORCEINLINE
WdfRequestMarkCancelableEx(
    _In_
    WDFREQUEST Request,
    _In_
    PFN_WDF_REQUEST_CANCEL EvtRequestCancel
    )
{
    return ((PFN_WDFREQUESTMARKCANCELABLEEX) WdfFunctions[WdfRequestMarkCancelableExTableIndex])(WdfDriverGlobals, Request, EvtRequestCancel);
}

//
// WDF Function: WdfRequestUnmarkCancelable
//
typedef
_Must_inspect_result_
_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
NTSTATUS
(*PFN_WDFREQUESTUNMARKCANCELABLE)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFREQUEST Request
    );

_Must_inspect_result_
_IRQL_requires_max_(DISPATCH_LEVEL)
NTSTATUS
FORCEINLINE
WdfRequestUnmarkCancelable(
    _In_
    WDFREQUEST Request
    )
{
    return ((PFN_WDFREQUESTUNMARKCANCELABLE) WdfFunctions[WdfRequestUnmarkCancelableTableIndex])(WdfDriverGlobals, Request);
}

//
// WDF Function: WdfRequestIsCanceled
//
typedef
_Must_inspect_result_
_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
BOOLEAN
(*PFN_WDFREQUESTISCANCELED)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFREQUEST Request
    );

_Must_inspect_result_
_IRQL_requires_max_(DISPATCH_LEVEL)
BOOLEAN
FORCEINLINE
WdfRequestIsCanceled(
    _In_
    WDFREQUEST Request
    )
{
    return ((PFN_WDFREQUESTISCANCELED) WdfFunctions[WdfRequestIsCanceledTableIndex])(WdfDriverGlobals, Request);
}

//
// WDF Function: WdfRequestCancelSentRequest
//
typedef
_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
BOOLEAN
(*PFN_WDFREQUESTCANCELSENTREQUEST)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFREQUEST Request
    );

_IRQL_requires_max_(DISPATCH_LEVEL)
BOOLEAN
FORCEINLINE
WdfRequestCancelSentRequest(
    _In_
    WDFREQUEST Request
    )
{
    return ((PFN_WDFREQUESTCANCELSENTREQUEST) WdfFunctions[WdfRequestCancelSentRequestTableIndex])(WdfDriverGlobals, Request);
}

//
// WDF Function: WdfRequestIsFrom32BitProcess
//
typedef
_Must_inspect_result_
_IRQL_requires_max_(APC_LEVEL)
WDFAPI
BOOLEAN
(*PFN_WDFREQUESTISFROM32BITPROCESS)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFREQUEST Request
    );

_Must_inspect_result_
_IRQL_requires_max_(APC_LEVEL)
BOOLEAN
FORCEINLINE
WdfRequestIsFrom32BitProcess(
    _In_
    WDFREQUEST Request
    )
{
    return ((PFN_WDFREQUESTISFROM32BITPROCESS) WdfFunctions[WdfRequestIsFrom32BitProcessTableIndex])(WdfDriverGlobals, Request);
}

//
// WDF Function: WdfRequestSetCompletionRoutine
//
typedef
_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
VOID
(*PFN_WDFREQUESTSETCOMPLETIONROUTINE)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFREQUEST Request,
    _In_opt_
    PFN_WDF_REQUEST_COMPLETION_ROUTINE CompletionRoutine,
    _In_opt_ __drv_aliasesMem
    WDFCONTEXT CompletionContext
    );

_IRQL_requires_max_(DISPATCH_LEVEL)
VOID
FORCEINLINE
WdfRequestSetCompletionRoutine(
    _In_
    WDFREQUEST Request,
    _In_opt_
    PFN_WDF_REQUEST_COMPLETION_ROUTINE CompletionRoutine,
    _In_opt_ __drv_aliasesMem
    WDFCONTEXT CompletionContext
    )
{
    ((PFN_WDFREQUESTSETCOMPLETIONROUTINE) WdfFunctions[WdfRequestSetCompletionRoutineTableIndex])(WdfDriverGlobals, Request, CompletionRoutine, CompletionContext);
}

//
// WDF Function: WdfRequestGetCompletionParams
//
typedef
_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
VOID
(*PFN_WDFREQUESTGETCOMPLETIONPARAMS)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFREQUEST Request,
    _Out_
    PWDF_REQUEST_COMPLETION_PARAMS Params
    );

_IRQL_requires_max_(DISPATCH_LEVEL)
VOID
FORCEINLINE
WdfRequestGetCompletionParams(
    _In_
    WDFREQUEST Request,
    _Out_
    PWDF_REQUEST_COMPLETION_PARAMS Params
    )
{
    ((PFN_WDFREQUESTGETCOMPLETIONPARAMS) WdfFunctions[WdfRequestGetCompletionParamsTableIndex])(WdfDriverGlobals, Request, Params);
}

//
// WDF Function: WdfRequestAllocateTimer
//
typedef
_Must_inspect_result_
_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
NTSTATUS
(*PFN_WDFREQUESTALLOCATETIMER)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFREQUEST Request
    );

_Must_inspect_result_
_IRQL_requires_max_(DISPATCH_LEVEL)
NTSTATUS
FORCEINLINE
WdfRequestAllocateTimer(
    _In_
    WDFREQUEST Request
    )
{
    return ((PFN_WDFREQUESTALLOCATETIMER) WdfFunctions[WdfRequestAllocateTimerTableIndex])(WdfDriverGlobals, Request);
}

//
// WDF Function: WdfRequestComplete
//
typedef
_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
VOID
(*PFN_WDFREQUESTCOMPLETE)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFREQUEST Request,
    _In_
    NTSTATUS Status
    );

_IRQL_requires_max_(DISPATCH_LEVEL)
VOID
FORCEINLINE
WdfRequestComplete(
    _In_
    WDFREQUEST Request,
    _In_
    NTSTATUS Status
    )
{
    ((PFN_WDFREQUESTCOMPLETE) WdfFunctions[WdfRequestCompleteTableIndex])(WdfDriverGlobals, Request, Status);
}

//
// WDF Function: WdfRequestCompleteWithPriorityBoost
//
typedef
_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
VOID
(*PFN_WDFREQUESTCOMPLETEWITHPRIORITYBOOST)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFREQUEST Request,
    _In_
    NTSTATUS Status,
    _In_
    CCHAR PriorityBoost
    );

_IRQL_requires_max_(DISPATCH_LEVEL)
VOID
FORCEINLINE
WdfRequestCompleteWithPriorityBoost(
    _In_
    WDFREQUEST Request,
    _In_
    NTSTATUS Status,
    _In_
    CCHAR PriorityBoost
    )
{
    ((PFN_WDFREQUESTCOMPLETEWITHPRIORITYBOOST) WdfFunctions[WdfRequestCompleteWithPriorityBoostTableIndex])(WdfDriverGlobals, Request, Status, PriorityBoost);
}

//
// WDF Function: WdfRequestCompleteWithInformation
//
typedef
_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
VOID
(*PFN_WDFREQUESTCOMPLETEWITHINFORMATION)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFREQUEST Request,
    _In_
    NTSTATUS Status,
    _In_
    ULONG_PTR Information
    );

_IRQL_requires_max_(DISPATCH_LEVEL)
VOID
FORCEINLINE
WdfRequestCompleteWithInformation(
    _In_
    WDFREQUEST Request,
    _In_
    NTSTATUS Status,
    _In_
    ULONG_PTR Information
    )
{
    ((PFN_WDFREQUESTCOMPLETEWITHINFORMATION) WdfFunctions[WdfRequestCompleteWithInformationTableIndex])(WdfDriverGlobals, Request, Status, Information);
}

//
// WDF Function: WdfRequestGetParameters
//
typedef
_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
VOID
(*PFN_WDFREQUESTGETPARAMETERS)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFREQUEST Request,
    _Out_
    PWDF_REQUEST_PARAMETERS Parameters
    );

_IRQL_requires_max_(DISPATCH_LEVEL)
VOID
FORCEINLINE
WdfRequestGetParameters(
    _In_
    WDFREQUEST Request,
    _Out_
    PWDF_REQUEST_PARAMETERS Parameters
    )
{
    ((PFN_WDFREQUESTGETPARAMETERS) WdfFunctions[WdfRequestGetParametersTableIndex])(WdfDriverGlobals, Request, Parameters);
}

//
// WDF Function: WdfRequestRetrieveInputMemory
//
typedef
_Must_inspect_result_
_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
NTSTATUS
(*PFN_WDFREQUESTRETRIEVEINPUTMEMORY)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFREQUEST Request,
    _Out_
    WDFMEMORY* Memory
    );

_Must_inspect_result_
_IRQL_requires_max_(DISPATCH_LEVEL)
NTSTATUS
FORCEINLINE
WdfRequestRetrieveInputMemory(
    _In_
    WDFREQUEST Request,
    _Out_
    WDFMEMORY* Memory
    )
{
    return ((PFN_WDFREQUESTRETRIEVEINPUTMEMORY) WdfFunctions[WdfRequestRetrieveInputMemoryTableIndex])(WdfDriverGlobals, Request, Memory);
}

//
// WDF Function: WdfRequestRetrieveOutputMemory
//
typedef
_Must_inspect_result_
_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
NTSTATUS
(*PFN_WDFREQUESTRETRIEVEOUTPUTMEMORY)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFREQUEST Request,
    _Out_
    WDFMEMORY* Memory
    );

_Must_inspect_result_
_IRQL_requires_max_(DISPATCH_LEVEL)
NTSTATUS
FORCEINLINE
WdfRequestRetrieveOutputMemory(
    _In_
    WDFREQUEST Request,
    _Out_
    WDFMEMORY* Memory
    )
{
    return ((PFN_WDFREQUESTRETRIEVEOUTPUTMEMORY) WdfFunctions[WdfRequestRetrieveOutputMemoryTableIndex])(WdfDriverGlobals, Request, Memory);
}

//
// WDF Function: WdfRequestRetrieveInputBuffer
//
typedef
_Must_inspect_result_
_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
NTSTATUS
(*PFN_WDFREQUESTRETRIEVEINPUTBUFFER)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFREQUEST Request,
    _In_
    size_t MinimumRequiredLength,
    _Outptr_result_bytebuffer_(*Length)
    PVOID* Buffer,
    _Out_opt_
    size_t* Length
    );

_Must_inspect_result_
_IRQL_requires_max_(DISPATCH_LEVEL)
NTSTATUS
FORCEINLINE
WdfRequestRetrieveInputBuffer(
    _In_
    WDFREQUEST Request,
    _In_
    size_t MinimumRequiredLength,
    _Outptr_result_bytebuffer_(*Length)
    PVOID* Buffer,
    _Out_opt_
    size_t* Length
    )
{
    return ((PFN_WDFREQUESTRETRIEVEINPUTBUFFER) WdfFunctions[WdfRequestRetrieveInputBufferTableIndex])(WdfDriverGlobals, Request, MinimumRequiredLength, Buffer, Length);
}

//
// WDF Function: WdfRequestRetrieveOutputBuffer
//
typedef
_Must_inspect_result_
_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
NTSTATUS
(*PFN_WDFREQUESTRETRIEVEOUTPUTBUFFER)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFREQUEST Request,
    _In_
    size_t MinimumRequiredSize,
    _Outptr_result_bytebuffer_(*Length)
    PVOID* Buffer,
    _Out_opt_
    size_t* Length
    );

_Must_inspect_result_
_IRQL_requires_max_(DISPATCH_LEVEL)
NTSTATUS
FORCEINLINE
WdfRequestRetrieveOutputBuffer(
    _In_
    WDFREQUEST Request,
    _In_
    size_t MinimumRequiredSize,
    _Outptr_result_bytebuffer_(*Length)
    PVOID* Buffer,
    _Out_opt_
    size_t* Length
    )
{
    return ((PFN_WDFREQUESTRETRIEVEOUTPUTBUFFER) WdfFunctions[WdfRequestRetrieveOutputBufferTableIndex])(WdfDriverGlobals, Request, MinimumRequiredSize, Buffer, Length);
}

//
// WDF Function: WdfRequestRetrieveInputWdmMdl
//
typedef
_Must_inspect_result_
_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
NTSTATUS
(*PFN_WDFREQUESTRETRIEVEINPUTWDMMDL)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFREQUEST Request,
    _Outptr_
    PMDL* Mdl
    );

_Must_inspect_result_
_IRQL_requires_max_(DISPATCH_LEVEL)
NTSTATUS
FORCEINLINE
WdfRequestRetrieveInputWdmMdl(
    _In_
    WDFREQUEST Request,
    _Outptr_
    PMDL* Mdl
    )
{
    return ((PFN_WDFREQUESTRETRIEVEINPUTWDMMDL) WdfFunctions[WdfRequestRetrieveInputWdmMdlTableIndex])(WdfDriverGlobals, Request, Mdl);
}

//
// WDF Function: WdfRequestRetrieveOutputWdmMdl
//
typedef
_Must_inspect_result_
_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
NTSTATUS
(*PFN_WDFREQUESTRETRIEVEOUTPUTWDMMDL)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFREQUEST Request,
    _Outptr_
    PMDL* Mdl
    );

_Must_inspect_result_
_IRQL_requires_max_(DISPATCH_LEVEL)
NTSTATUS
FORCEINLINE
WdfRequestRetrieveOutputWdmMdl(
    _In_
    WDFREQUEST Request,
    _Outptr_
    PMDL* Mdl
    )
{
    return ((PFN_WDFREQUESTRETRIEVEOUTPUTWDMMDL) WdfFunctions[WdfRequestRetrieveOutputWdmMdlTableIndex])(WdfDriverGlobals, Request, Mdl);
}

//
// WDF Function: WdfRequestRetrieveUnsafeUserInputBuffer
//
typedef
_Must_inspect_result_
_IRQL_requires_max_(PASSIVE_LEVEL)
WDFAPI
NTSTATUS
(*PFN_WDFREQUESTRETRIEVEUNSAFEUSERINPUTBUFFER)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFREQUEST Request,
    _In_
    size_t MinimumRequiredLength,
    _Outptr_result_bytebuffer_maybenull_(*Length)
    PVOID* InputBuffer,
    _Out_opt_
    size_t* Length
    );

_Must_inspect_result_
_IRQL_requires_max_(PASSIVE_LEVEL)
NTSTATUS
FORCEINLINE
WdfRequestRetrieveUnsafeUserInputBuffer(
    _In_
    WDFREQUEST Request,
    _In_
    size_t MinimumRequiredLength,
    _Outptr_result_bytebuffer_maybenull_(*Length)
    PVOID* InputBuffer,
    _Out_opt_
    size_t* Length
    )
{
    return ((PFN_WDFREQUESTRETRIEVEUNSAFEUSERINPUTBUFFER) WdfFunctions[WdfRequestRetrieveUnsafeUserInputBufferTableIndex])(WdfDriverGlobals, Request, MinimumRequiredLength, InputBuffer, Length);
}

//
// WDF Function: WdfRequestRetrieveUnsafeUserOutputBuffer
//
typedef
_Must_inspect_result_
_IRQL_requires_max_(PASSIVE_LEVEL)
WDFAPI
NTSTATUS
(*PFN_WDFREQUESTRETRIEVEUNSAFEUSEROUTPUTBUFFER)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFREQUEST Request,
    _In_
    size_t MinimumRequiredLength,
    _Outptr_result_bytebuffer_maybenull_(*Length)
    PVOID* OutputBuffer,
    _Out_opt_
    size_t* Length
    );

_Must_inspect_result_
_IRQL_requires_max_(PASSIVE_LEVEL)
NTSTATUS
FORCEINLINE
WdfRequestRetrieveUnsafeUserOutputBuffer(
    _In_
    WDFREQUEST Request,
    _In_
    size_t MinimumRequiredLength,
    _Outptr_result_bytebuffer_maybenull_(*Length)
    PVOID* OutputBuffer,
    _Out_opt_
    size_t* Length
    )
{
    return ((PFN_WDFREQUESTRETRIEVEUNSAFEUSEROUTPUTBUFFER) WdfFunctions[WdfRequestRetrieveUnsafeUserOutputBufferTableIndex])(WdfDriverGlobals, Request, MinimumRequiredLength, OutputBuffer, Length);
}

//
// WDF Function: WdfRequestSetInformation
//
typedef
_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
VOID
(*PFN_WDFREQUESTSETINFORMATION)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFREQUEST Request,
    _In_
    ULONG_PTR Information
    );

_IRQL_requires_max_(DISPATCH_LEVEL)
VOID
FORCEINLINE
WdfRequestSetInformation(
    _In_
    WDFREQUEST Request,
    _In_
    ULONG_PTR Information
    )
{
    ((PFN_WDFREQUESTSETINFORMATION) WdfFunctions[WdfRequestSetInformationTableIndex])(WdfDriverGlobals, Request, Information);
}

//
// WDF Function: WdfRequestGetInformation
//
typedef
_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
ULONG_PTR
(*PFN_WDFREQUESTGETINFORMATION)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFREQUEST Request
    );

_IRQL_requires_max_(DISPATCH_LEVEL)
ULONG_PTR
FORCEINLINE
WdfRequestGetInformation(
    _In_
    WDFREQUEST Request
    )
{
    return ((PFN_WDFREQUESTGETINFORMATION) WdfFunctions[WdfRequestGetInformationTableIndex])(WdfDriverGlobals, Request);
}

//
// WDF Function: WdfRequestGetFileObject
//
typedef
_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
WDFFILEOBJECT
(*PFN_WDFREQUESTGETFILEOBJECT)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFREQUEST Request
    );

_IRQL_requires_max_(DISPATCH_LEVEL)
WDFFILEOBJECT
FORCEINLINE
WdfRequestGetFileObject(
    _In_
    WDFREQUEST Request
    )
{
    return ((PFN_WDFREQUESTGETFILEOBJECT) WdfFunctions[WdfRequestGetFileObjectTableIndex])(WdfDriverGlobals, Request);
}

//
// WDF Function: WdfRequestProbeAndLockUserBufferForRead
//
typedef
_Must_inspect_result_
_IRQL_requires_max_(PASSIVE_LEVEL)
WDFAPI
NTSTATUS
(*PFN_WDFREQUESTPROBEANDLOCKUSERBUFFERFORREAD)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFREQUEST Request,
    _In_reads_bytes_(Length)
    PVOID Buffer,
    _In_
    size_t Length,
    _Out_
    WDFMEMORY* MemoryObject
    );

_Must_inspect_result_
_IRQL_requires_max_(PASSIVE_LEVEL)
NTSTATUS
FORCEINLINE
WdfRequestProbeAndLockUserBufferForRead(
    _In_
    WDFREQUEST Request,
    _In_reads_bytes_(Length)
    PVOID Buffer,
    _In_
    size_t Length,
    _Out_
    WDFMEMORY* MemoryObject
    )
{
    return ((PFN_WDFREQUESTPROBEANDLOCKUSERBUFFERFORREAD) WdfFunctions[WdfRequestProbeAndLockUserBufferForReadTableIndex])(WdfDriverGlobals, Request, Buffer, Length, MemoryObject);
}

//
// WDF Function: WdfRequestProbeAndLockUserBufferForWrite
//
typedef
_Must_inspect_result_
_IRQL_requires_max_(PASSIVE_LEVEL)
WDFAPI
NTSTATUS
(*PFN_WDFREQUESTPROBEANDLOCKUSERBUFFERFORWRITE)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFREQUEST Request,
    _In_reads_bytes_(Length)
    PVOID Buffer,
    _In_
    size_t Length,
    _Out_
    WDFMEMORY* MemoryObject
    );

_Must_inspect_result_
_IRQL_requires_max_(PASSIVE_LEVEL)
NTSTATUS
FORCEINLINE
WdfRequestProbeAndLockUserBufferForWrite(
    _In_
    WDFREQUEST Request,
    _In_reads_bytes_(Length)
    PVOID Buffer,
    _In_
    size_t Length,
    _Out_
    WDFMEMORY* MemoryObject
    )
{
    return ((PFN_WDFREQUESTPROBEANDLOCKUSERBUFFERFORWRITE) WdfFunctions[WdfRequestProbeAndLockUserBufferForWriteTableIndex])(WdfDriverGlobals, Request, Buffer, Length, MemoryObject);
}

//
// WDF Function: WdfRequestGetRequestorMode
//
typedef
_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
KPROCESSOR_MODE
(*PFN_WDFREQUESTGETREQUESTORMODE)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFREQUEST Request
    );

_IRQL_requires_max_(DISPATCH_LEVEL)
KPROCESSOR_MODE
FORCEINLINE
WdfRequestGetRequestorMode(
    _In_
    WDFREQUEST Request
    )
{
    return ((PFN_WDFREQUESTGETREQUESTORMODE) WdfFunctions[WdfRequestGetRequestorModeTableIndex])(WdfDriverGlobals, Request);
}

//
// WDF Function: WdfRequestForwardToIoQueue
//
typedef
_Must_inspect_result_
_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
NTSTATUS
(*PFN_WDFREQUESTFORWARDTOIOQUEUE)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFREQUEST Request,
    _In_
    WDFQUEUE DestinationQueue
    );

_Must_inspect_result_
_IRQL_requires_max_(DISPATCH_LEVEL)
NTSTATUS
FORCEINLINE
WdfRequestForwardToIoQueue(
    _In_
    WDFREQUEST Request,
    _In_
    WDFQUEUE DestinationQueue
    )
{
    return ((PFN_WDFREQUESTFORWARDTOIOQUEUE) WdfFunctions[WdfRequestForwardToIoQueueTableIndex])(WdfDriverGlobals, Request, DestinationQueue);
}

//
// WDF Function: WdfRequestGetIoQueue
//
typedef
_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
WDFQUEUE
(*PFN_WDFREQUESTGETIOQUEUE)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFREQUEST Request
    );

_IRQL_requires_max_(DISPATCH_LEVEL)
WDFQUEUE
FORCEINLINE
WdfRequestGetIoQueue(
    _In_
    WDFREQUEST Request
    )
{
    return ((PFN_WDFREQUESTGETIOQUEUE) WdfFunctions[WdfRequestGetIoQueueTableIndex])(WdfDriverGlobals, Request);
}

//
// WDF Function: WdfRequestRequeue
//
typedef
_Must_inspect_result_
_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
NTSTATUS
(*PFN_WDFREQUESTREQUEUE)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFREQUEST Request
    );

_Must_inspect_result_
_IRQL_requires_max_(DISPATCH_LEVEL)
NTSTATUS
FORCEINLINE
WdfRequestRequeue(
    _In_
    WDFREQUEST Request
    )
{
    return ((PFN_WDFREQUESTREQUEUE) WdfFunctions[WdfRequestRequeueTableIndex])(WdfDriverGlobals, Request);
}

//
// WDF Function: WdfRequestStopAcknowledge
//
typedef
_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
VOID
(*PFN_WDFREQUESTSTOPACKNOWLEDGE)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFREQUEST Request,
    _In_
    BOOLEAN Requeue
    );

_IRQL_requires_max_(DISPATCH_LEVEL)
VOID
FORCEINLINE
WdfRequestStopAcknowledge(
    _In_
    WDFREQUEST Request,
    _In_
    BOOLEAN Requeue
    )
{
    ((PFN_WDFREQUESTSTOPACKNOWLEDGE) WdfFunctions[WdfRequestStopAcknowledgeTableIndex])(WdfDriverGlobals, Request, Requeue);
}

//
// WDF Function: WdfRequestWdmGetIrp
//
typedef
_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
PIRP
(*PFN_WDFREQUESTWDMGETIRP)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFREQUEST Request
    );

_IRQL_requires_max_(DISPATCH_LEVEL)
PIRP
FORCEINLINE
WdfRequestWdmGetIrp(
    _In_
    WDFREQUEST Request
    )
{
    return ((PFN_WDFREQUESTWDMGETIRP) WdfFunctions[WdfRequestWdmGetIrpTableIndex])(WdfDriverGlobals, Request);
}

//
// WDF Function: WdfRequestIsReserved
//
typedef
_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
BOOLEAN
(*PFN_WDFREQUESTISRESERVED)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFREQUEST Request
    );

_IRQL_requires_max_(DISPATCH_LEVEL)
BOOLEAN
FORCEINLINE
WdfRequestIsReserved(
    _In_
    WDFREQUEST Request
    )
{
    return ((PFN_WDFREQUESTISRESERVED) WdfFunctions[WdfRequestIsReservedTableIndex])(WdfDriverGlobals, Request);
}

//
// WDF Function: WdfRequestForwardToParentDeviceIoQueue
//
typedef
_Must_inspect_result_
_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
NTSTATUS
(*PFN_WDFREQUESTFORWARDTOPARENTDEVICEIOQUEUE)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFREQUEST Request,
    _In_
    WDFQUEUE ParentDeviceQueue,
    _In_
    PWDF_REQUEST_FORWARD_OPTIONS ForwardOptions
    );

_Must_inspect_result_
_IRQL_requires_max_(DISPATCH_LEVEL)
NTSTATUS
FORCEINLINE
WdfRequestForwardToParentDeviceIoQueue(
    _In_
    WDFREQUEST Request,
    _In_
    WDFQUEUE ParentDeviceQueue,
    _In_
    PWDF_REQUEST_FORWARD_OPTIONS ForwardOptions
    )
{
    return ((PFN_WDFREQUESTFORWARDTOPARENTDEVICEIOQUEUE) WdfFunctions[WdfRequestForwardToParentDeviceIoQueueTableIndex])(WdfDriverGlobals, Request, ParentDeviceQueue, ForwardOptions);
}



#endif // (NTDDI_VERSION >= NTDDI_WIN2K)


#endif // _WDFREQUEST_1_9_H_
