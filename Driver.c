/*++

Module Name:

    driver.c

Abstract:

    This file contains the driver entry points and callbacks.

Environment:

    Kernel-mode Driver Framework

Author:
    
    Olivier Lesage
--*/

#include "driver.h"
#include "driver.tmh"

// stores data required for work
typedef struct _SAMPLE_DEVICE_EXTENSION
{
    int info;
}SAMPLE_DEVICE_EXTENSION, *PSAMPLE_DEVICE_EXTENSION;

#ifdef ALLOC_PRAGMA
#pragma alloc_text (INIT, DriverEntry)
#pragma alloc_text (PAGE, WacomPracticeEvtDeviceAdd)
#pragma alloc_text (PAGE, WacomPracticeEvtDriverContextCleanup)
#endif

UNICODE_STRING DEVICE_NAME = RTL_CONSTANT_STRING(L"\\Device\\WacomPracticeDevice");
UNICODE_STRING DEVICE_SYMBOLIC_NAME = RTL_CONSTANT_STRING(L"\\??\\WacomPracticeDeviceLink");

NTSTATUS MajorFunctions
(
    PDEVICE_OBJECT DeviceObject,
    PIRP Irp
)
{
    UNREFERENCED_PARAMETER(DeviceObject);
    // get the stack location of the current I/O request packet
    PIO_STACK_LOCATION stackLocation = NULL;
    stackLocation = IoGetCurrentIrpStackLocation(Irp);

    switch (stackLocation->MajorFunction)
    {
    case IRP_MJ_CREATE:
        KdPrint(("Handle to symbolic link %wZ opened", DEVICE_SYMBOLIC_NAME));
        break;
    case IRP_MJ_CLOSE:
        KdPrint(("Handle to symbolic link %wZ clsoed", DEVICE_SYMBOLIC_NAME));
        break;
    default:
        break;
    }

    Irp->IoStatus.Information = 0;
    Irp->IoStatus.Status = STATUS_SUCCESS;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);

    return STATUS_SUCCESS;
}

NTSTATUS HandleCustomIOCTL
(
    PDEVICE_OBJECT DeviceObject,
    PIRP Irp
)
{
    UNREFERENCED_PARAMETER(DeviceObject);
    UNREFERENCED_PARAMETER(Irp);
    //PIO_STACK_LOCATION stackLocation = NULL;
    return STATUS_SUCCESS;
}

NTSTATUS DriverEntry(
    _In_ PDRIVER_OBJECT  DriverObject,
    _In_ PUNICODE_STRING RegistryPath
    )
/*++

Routine Description:
    DriverEntry initializes the driver and is the first routine called by the
    system after the driver is loaded. DriverEntry specifies the other entry
    points in the function driver, such as EvtDevice and DriverUnload.

Parameters Description:

    DriverObject - represents the instance of the function driver that is loaded
    into memory. DriverEntry must initialize members of DriverObject before it
    returns to the caller. DriverObject is allocated by the system before the
    driver is loaded, and it is released by the system after the system unloads
    the function driver from memory.

    RegistryPath - represents the driver specific path in the Registry.
    The function driver can use the path to store driver related data between
    reboots. The path does not store hardware instance specific data.

Return Value:

    STATUS_SUCCESS if successful,
    STATUS_UNSUCCESSFUL otherwise.

--*/
{
    WDF_DRIVER_CONFIG config;
    NTSTATUS status = 0;
    WDF_OBJECT_ATTRIBUTES attributes;

    KdPrint(("DEBUG: Driver was entered.\n"));
    //
    // Initialize WPP Tracing
    //
    WPP_INIT_TRACING(DriverObject, RegistryPath);

    TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "%!FUNC! Entry");

    // set up routine for driver unloading
    //DriverObject->DriverUnload = DriverUnload;

    // set up routine for handling IO requests from applications
    //DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = HandleCustomIOCTL;

    // set up routines that will execute once the device's symbolic link is opened/closed
    DriverObject->MajorFunction[IRP_MJ_CREATE] = MajorFunctions;
    DriverObject->MajorFunction[IRP_MJ_CLOSE] = MajorFunctions;

    status = IoCreateDevice(DriverObject, 0, &DEVICE_NAME, FILE_DEVICE_UNKNOWN, FILE_DEVICE_SECURE_OPEN, FALSE, &DriverObject->DeviceObject);
    if (!NT_SUCCESS(status)) KdPrint(("Could not create device %wZ", DEVICE_NAME));
    else KdPrint(("Device %wZ created", DEVICE_NAME));

    status = IoCreateSymbolicLink(&DEVICE_SYMBOLIC_NAME, &DEVICE_NAME);
    if (!NT_SUCCESS(status)) KdPrint(("Error creating symbolic link %wZ", DEVICE_SYMBOLIC_NAME));
    else KdPrint(("Symbolic link %wZ created", DEVICE_SYMBOLIC_NAME));

    //
    // Register a cleanup callback so that we can call WPP_CLEANUP when
    // the framework driver object is deleted during driver unload.
    //
    WDF_OBJECT_ATTRIBUTES_INIT(&attributes);
    attributes.EvtCleanupCallback = WacomPracticeEvtDriverContextCleanup;

    WDF_DRIVER_CONFIG_INIT(&config,
                           WacomPracticeEvtDeviceAdd
                           );

    status = WdfDriverCreate(DriverObject,
                             RegistryPath,
                             &attributes,
                             &config,
                             WDF_NO_HANDLE
                             );

    if (!NT_SUCCESS(status)) {
        TraceEvents(TRACE_LEVEL_ERROR, TRACE_DRIVER, "WdfDriverCreate failed %!STATUS!", status);
        WPP_CLEANUP(DriverObject);
        return status;
    }

    TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "%!FUNC! Exit");

    return status;
}

NTSTATUS
WacomPracticeEvtDeviceAdd(
    _In_    WDFDRIVER       Driver,
    _Inout_ PWDFDEVICE_INIT DeviceInit
    )
/*++
Routine Description:

    EvtDeviceAdd is called by the framework in response to AddDevice
    call from the PnP manager. We create and initialize a device object to
    represent a new instance of the device.

Arguments:

    Driver - Handle to a framework driver object created in DriverEntry

    DeviceInit - Pointer to a framework-allocated WDFDEVICE_INIT structure.

Return Value:

    NTSTATUS

--*/
{
    NTSTATUS status;

    UNREFERENCED_PARAMETER(Driver);

    PAGED_CODE();

    TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "%!FUNC! Entry");

    status = WacomPracticeCreateDevice(DeviceInit);

    TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "%!FUNC! Exit");

    return status;
}

VOID
WacomPracticeEvtDriverContextCleanup(
    _In_ WDFOBJECT DriverObject
    )
/*++
Routine Description:

    Free all the resources allocated in DriverEntry.

Arguments:

    DriverObject - handle to a WDF Driver object.

Return Value:

    VOID.

--*/
{
    UNREFERENCED_PARAMETER(DriverObject);

    PAGED_CODE ();

    TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "%!FUNC! Entry");

    //
    // Stop WPP Tracing
    //
    WPP_CLEANUP( WdfDriverWdmGetDriverObject( (WDFDRIVER) DriverObject) );

}
