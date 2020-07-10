/*++

Module Name:

    public.h

Abstract:

    This module contains the common declarations shared by driver
    and user applications.

Environment:

    user and kernel

--*/

//
// Define an Interface Guid so that app can find the device and talk to it.
//

DEFINE_GUID (GUID_DEVINTERFACE_WacomPractice,
    0xcfab8b22,0x3ad1,0x4edf,0x86,0x26,0xd2,0xc7,0xe2,0x92,0xe0,0x4d);
// {cfab8b22-3ad1-4edf-8626-d2c7e292e04d}
