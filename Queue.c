/*++

Module Name:

    queue.c

Abstract:

    This file contains the queue entry points and callbacks.

Environment:

    Kernel-mode Driver Framework

--*/

#include "driver.h"
#include "queue.tmh"

#ifdef ALLOC_PRAGMA
#pragma alloc_text (PAGE, WacomPracticeQueueInitialize)
#endif

NTSTATUS
WacomPracticeQueueInitialize(
    _In_ WDFDEVICE Device
    )
/*++

Routine Description:


     The I/O dispatch callbacks for the frameworks device object
     are configured in this function.

     A single default I/O Queue is configured for parallel request
     processing, and a driver context memory allocation is created
     to hold our structure QUEUE_CONTEXT.

Arguments:

    Device - Handle to a framework device object.

Return Value:

    VOID

--*/
{
    WDFQUEUE queue;
    NTSTATUS status;
    WDF_IO_QUEUE_CONFIG    queueConfig;

    PAGED_CODE();
    
    //
    // Configure a default queue so that requests that are not
    // configure-fowarded using WdfDeviceConfigureRequestDispatching to goto
    // other queues get dispatched here.
    //
    WDF_IO_QUEUE_CONFIG_INIT_DEFAULT_QUEUE(
         &queueConfig,
        WdfIoQueueDispatchParallel
        );

    queueConfig.EvtIoDeviceControl = WacomPracticeEvtIoDeviceControl;
    queueConfig.EvtIoStop = WacomPracticeEvtIoStop;

    status = WdfIoQueueCreate(
                 Device,
                 &queueConfig,
                 WDF_NO_OBJECT_ATTRIBUTES,
                 &queue
                 );

    if( !NT_SUCCESS(status) ) {
        TraceEvents(TRACE_LEVEL_ERROR, TRACE_QUEUE, "WdfIoQueueCreate failed %!STATUS!", status);
        return status;
    }

    return status;
}

VOID
WacomPracticeEvtIoDeviceControl(
    _In_ WDFQUEUE Queue,
    _In_ WDFREQUEST Request,
    _In_ size_t OutputBufferLength,
    _In_ size_t InputBufferLength,
    _In_ ULONG IoControlCode
    )
/*++

Routine Description:

    This event is invoked when the framework receives IRP_MJ_DEVICE_CONTROL request.

Arguments:

    Queue -  Handle to the framework queue object that is associated with the
             I/O request.

    Request - Handle to a framework request object.

    OutputBufferLength - Size of the output buffer in bytes

    InputBufferLength - Size of the input buffer in bytes

    IoControlCode - I/O control code.

Return Value:

    VOID

--*/
{
    NTSTATUS status;
    TraceEvents(TRACE_LEVEL_INFORMATION, 
                TRACE_QUEUE, 
                "%!FUNC! Queue 0x%p, Request 0x%p OutputBufferLength %d InputBufferLength %d IoControlCode %d", 
                Queue, Request, (int) OutputBufferLength, (int) InputBufferLength, IoControlCode);

    KdPrint(("WacomPracticeEvtIoDeviceControl entered."));

    PDEVICE_CONTEXT pDeviceContext = DeviceGetContext(WdfIoQueueGetDevice(Queue));
    INT32* rgb;
    int h, s, v;
    PVOID buffer;
    size_t bufSize;

    KdPrint(("Received Control Code %d with Request = 0x%p\n", IoControlCode, Request));

    switch (IoControlCode) {

    case IOCTL_UWIRE_SETLED:
        status = WdfRequestRetrieveInputBuffer(Request, sizeof(INT32), &buffer, &bufSize);
        if (!NT_SUCCESS(status)) {
            KdPrint(("Failed to retrieve input buffer."));
            break;
        }
        rgb = (INT32*)buffer;
        *rgb = *rgb & 0x00FFFFFF; // mask top 8 bits (just in case)
        KdPrint(("Found value of 0x%x in the input buffer.\n", *rgb));

        SetLEDColor(pDeviceContext, *rgb & 0x0000FF, (*rgb & 0x00FF00) >> 8, (*rgb & 0xFF0000) >> 16);
        break;

    case IOCTL_UWIRE_BLINKLED:
        status = WdfRequestRetrieveInputBuffer(Request, sizeof(INT32), &buffer, &bufSize);
        if (!NT_SUCCESS(status)) {
            KdPrint(("Failed to retrieve input buffer."));
            break;
        }
        rgb = (INT32*)buffer;
        KdPrint(("Found value of 0x%x in the input buffer.\n", *rgb));
        BlinkLEDColor(pDeviceContext, *rgb & 0x0000FF, (*rgb & 0x00FF00)>>8, (*rgb & 0xFF0000)>>16, (*rgb & 0xFF000000) >> 24);
        break;

    case IOCTL_UWIRE_FADELED:
        status = WdfRequestRetrieveInputBuffer(Request, sizeof(INT32), &buffer, &bufSize);
        if (!NT_SUCCESS(status)) {
            KdPrint(("Failed to retrieve input buffer."));
            break;
        }
        rgb = (INT32*)buffer;
        KdPrint(("Found value of 0x%x in the input buffer.\n", *rgb));
        RGBtoHSV(&h, &s, &v, (*rgb & 0x00FF00) >> 8, (*rgb & 0xFF0000) >> 16, (*rgb & 0xFF000000) >> 24);
        CycleLEDColor(pDeviceContext, *rgb & 0x0000FF, h, s);
        break;
    default:
        KdPrint(("WARNING: IOCTL code not supported!\n"));
        break;
    }

    WdfRequestComplete(Request, STATUS_SUCCESS);

    return;
}

VOID
WacomPracticeEvtIoStop(
    _In_ WDFQUEUE Queue,
    _In_ WDFREQUEST Request,
    _In_ ULONG ActionFlags
)
/*++

Routine Description:

    This event is invoked for a power-managed queue before the device leaves the working state (D0).

Arguments:

    Queue -  Handle to the framework queue object that is associated with the
             I/O request.

    Request - Handle to a framework request object.

    ActionFlags - A bitwise OR of one or more WDF_REQUEST_STOP_ACTION_FLAGS-typed flags
                  that identify the reason that the callback function is being called
                  and whether the request is cancelable.

Return Value:

    VOID

--*/
{
    TraceEvents(TRACE_LEVEL_INFORMATION, 
                TRACE_QUEUE, 
                "%!FUNC! Queue 0x%p, Request 0x%p ActionFlags %d", 
                Queue, Request, ActionFlags);

    //
    // In most cases, the EvtIoStop callback function completes, cancels, or postpones
    // further processing of the I/O request.
    //
    // Typically, the driver uses the following rules:
    //
    // - If the driver owns the I/O request, it either postpones further processing
    //   of the request and calls WdfRequestStopAcknowledge, or it calls WdfRequestComplete
    //   with a completion status value of STATUS_SUCCESS or STATUS_CANCELLED.
    //  
    //   The driver must call WdfRequestComplete only once, to either complete or cancel
    //   the request. To ensure that another thread does not call WdfRequestComplete
    //   for the same request, the EvtIoStop callback must synchronize with the driver's
    //   other event callback functions, for instance by using interlocked operations.
    //
    // - If the driver has forwarded the I/O request to an I/O target, it either calls
    //   WdfRequestCancelSentRequest to attempt to cancel the request, or it postpones
    //   further processing of the request and calls WdfRequestStopAcknowledge.
    //
    // A driver might choose to take no action in EvtIoStop for requests that are
    // guaranteed to complete in a small amount of time. For example, the driver might
    // take no action for requests that are completed in one of the driver’s request handlers.
    //

    return;
}
