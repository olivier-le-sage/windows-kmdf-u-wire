/*++

Module Name:

    driver.h

Abstract:

    This file contains the driver definitions.

Environment:

    Kernel-mode Driver Framework

--*/

#include <ntddk.h>
#include <wdf.h>
#include <usb.h>
#include <usbdlib.h>
#include <wdfusb.h>
#include <initguid.h>

#include "device.h"
#include "queue.h"
#include "trace.h"
#include "HSVtoRGB.h"

EXTERN_C_START

//
// IOCTL code definitions
//
#define IOCTL_UWIRE_SETLED CTL_CODE(FILE_DEVICE_UNKNOWN, 0x2049, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_UWIRE_BLINKLED CTL_CODE(FILE_DEVICE_UNKNOWN, 0x204a, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_UWIRE_FADELED CTL_CODE(FILE_DEVICE_UNKNOWN, 0x204b, METHOD_BUFFERED, FILE_ANY_ACCESS)

//
// WDFDRIVER Events
//

DRIVER_INITIALIZE DriverEntry;
EVT_WDF_DRIVER_DEVICE_ADD WacomPracticeEvtDeviceAdd;
EVT_WDF_OBJECT_CONTEXT_CLEANUP WacomPracticeEvtDriverContextCleanup;

EXTERN_C_END
