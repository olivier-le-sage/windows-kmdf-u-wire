/*
 * Description:
 *    A set of c functions designed to be loaded by python to interact with a
 *    windows driver.
 *
 * Author:
 *    Olivier Lesage
 */

#include "led_control_app.h"

#define DEVICE_INST_PATH L"\\\\?\\USB#VID_1781&PID_0C9F#5&1E98413&0&2#{cfab8b22-3ad1-4edf-8626-d2c7e292e04d}"
//#define DEVICE_GUID L"cfab8b22-3ad1-4edf-8626-d2c7e292e04d"
//#define USB_CLASS_GUID L"88BAE032-5A81-49f0-BC3D-A4FF138216D6"

DEFINE_GUID (GUID_DEVINTERFACE_WacomPractice,
    0xcfab8b22,0x3ad1,0x4edf,0x86,0x26,0xd2,0xc7,0xe2,0x92,0xe0,0x4d);

typedef struct RGB_data {
    unsigned char r;
    unsigned char g;
    unsigned char b;
} RGB_data;

#define IOCTL_UWIRE_SETLED CTL_CODE(FILE_DEVICE_UNKNOWN, 0x2049, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_UWIRE_BLINKLED CTL_CODE(FILE_DEVICE_UNKNOWN, 0x204a, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_UWIRE_FADELED CTL_CODE(FILE_DEVICE_UNKNOWN, 0x204b, METHOD_BUFFERED, FILE_ANY_ACCESS)

static CHAR* find_device(void) {
    //CHAR* device_path;
    DWORD dwSize;
    DWORD dwMemberIdx = 0;
    SP_DEVICE_INTERFACE_DATA DevIntfData;
    PSP_DEVICE_INTERFACE_DETAIL_DATA DevIntfDetailData;
    SP_DEVINFO_DATA DevData;

    //device_path = (CHAR*)malloc(128);// allocate return string to the heap so that it won't be cleared
    //
    // Find the USB device path
    //
    HDEVINFO hDevInfo = SetupDiGetClassDevs(&GUID_DEVINTERFACE_WacomPractice,
                                            NULL,
                                            NULL,
                                            DIGCF_PRESENT|DIGCF_DEVICEINTERFACE);

    if (hDevInfo == INVALID_HANDLE_VALUE) {
        printf("SetupDiGetClassDevs() failed. Error: %ld", GetLastError());
        return NULL;
    }
    SetupDiEnumDeviceInterfaces(hDevInfo, NULL, &GUID_DEVINTERFACE_WacomPractice, dwMemberIdx, &DevIntfData);
    // get the required buffer size and store it in dwSize using a NULL call
    DevIntfData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);
    SetupDiGetDeviceInterfaceDetail(hDevInfo, &DevIntfData, NULL, 0, &dwSize, NULL);
    DevIntfDetailData = (PSP_DEVICE_INTERFACE_DETAIL_DATA)malloc(dwSize);
    DevIntfDetailData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
    // now perform the real call
    SetupDiGetDeviceInterfaceDetail(hDevInfo, &DevIntfData, DevIntfDetailData, dwSize, &dwSize, &DevData);

    return DevIntfDetailData->DevicePath; // the device path
}

BOOL uwire_set_led_color(unsigned char r, unsigned char g, unsigned char b) {
    HANDLE device = INVALID_HANDLE_VALUE;
    BOOL status = FALSE;
    DWORD bytesReturned = 0;
    CHAR inBuffer[16] = {0};  // was [128]
    CHAR outBuffer[16] = {0}; // was [128]
    CHAR* device_path;

    // find the device
    device_path = find_device();
    if (device_path == NULL) {
        printf("Failed to elaborate device path string.\n");
        return FALSE;
    } else {
        printf("Found device path string: %s\n", device_path);
    }

    // send the data in as a 24-bit lump of data -- the driver will know how to deal with it
    RGB_data* rgb = (RGB_data*)malloc(sizeof(RGB_data));
    rgb->r = r;
    rgb->g = g;
    rgb->b = b;
    RtlCopyMemory(inBuffer, rgb, sizeof(RGB_data)); // same as memcpy()

    device = CreateFile(device_path,
                        GENERIC_ALL,
                        0,
                        0,
                        OPEN_EXISTING,
                        FILE_ATTRIBUTE_NORMAL,
                        0);

    if (device == INVALID_HANDLE_VALUE) {
        long unsigned int errorCode = GetLastError();
        if (errorCode == ERROR_ACCESS_DENIED) printf("> Device found but access was denied! Try running as admin.\n");
        printf("> Could not open device. Error Code = 0x%lx\n", errorCode);
        return FALSE;
    }

    printf("> Issuing IOCTL_UWIRE_SETLED 0x%x\n", IOCTL_UWIRE_SETLED);
    status = DeviceIoControl(device,
                             IOCTL_UWIRE_SETLED,
                             inBuffer,
                             sizeof(inBuffer),
                             outBuffer,
                             sizeof(outBuffer),
                             &bytesReturned,
                             (LPOVERLAPPED) NULL);

    printf("> IOCTL_UWIRE_SETLED 0x%x issued\n", IOCTL_UWIRE_SETLED);
    printf("> Returned by the kernel: %s. Received buffer size: %lu\n", outBuffer, bytesReturned);

    CloseHandle(device);
    return status;
}

BOOL uwire_blink_led(int duration) {
    // open the USB device
    HANDLE device = INVALID_HANDLE_VALUE;
    BOOL status = FALSE;
    DWORD bytesReturned = 0;
    CHAR inBuffer[128] = {0};
    CHAR outBuffer[128] = {0};
    CHAR* device_path;

    // find the device
    device_path = find_device();
    if (device_path == NULL) {
        printf("Failed to elaborate device path string.\n");
        return FALSE;
    } else {
        printf("Found device path string: %s\n", device_path);
    }

    printf("DEBUG: Sending in (buffer contents) 0x%x\n", duration);
    RtlCopyMemory(inBuffer, &duration, sizeof(duration));
    device = CreateFile(device_path,
                        GENERIC_ALL,
                        0,
                        0,
                        OPEN_EXISTING,
                        FILE_ATTRIBUTE_SYSTEM,
                        0);

    if (device == INVALID_HANDLE_VALUE) {
        printf("> Could not open device: 0x%lx\n", GetLastError());
        return FALSE;
    }

    printf("> Issuing IOCTL_UWIRE_BLINKLED 0x%x\n", IOCTL_UWIRE_BLINKLED);
    status = DeviceIoControl(device,
                             IOCTL_UWIRE_BLINKLED,
                             inBuffer,
                             sizeof(inBuffer),
                             outBuffer,
                             sizeof(outBuffer),
                             &bytesReturned,
                             (LPOVERLAPPED) NULL);

    printf("> IOCTL_UWIRE_BLINKLED 0x%x issued\n", IOCTL_UWIRE_BLINKLED);
    printf("> Returned by the kernel: %s. Received buffer size: %lu\n", outBuffer, bytesReturned);

    CloseHandle(device);
    return status;
}
