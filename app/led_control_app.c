/*
 * Description:
 *    A set of c functions designed to be loaded by python to interact with a
 *    windows driver.
 *
 * Author:
 *    Olivier Lesage
 */

#include "led_control_app.h"

typedef struct RGB_data {
    unsigned char r;
    unsigned char g;
    unsigned char b;
} RGB_data;

#define IOCTL_UWIRE_SETLED CTL_CODE(FILE_DEVICE_UNKNOWN, 0x2049, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_UWIRE_BLINKLED CTL_CODE(FILE_DEVICE_UNKNOWN, 0x204a, METHOD_BUFFERED, FILE_ANY_ACCESS)

BOOL uwire_set_led_color(unsigned char r, unsigned char g, unsigned char b) {
    // open the USB device
    HANDLE device = INVALID_HANDLE_VALUE;
    BOOL status = FALSE;
    DWORD bytesReturned = 0;
    CHAR inBuffer[16] = {0};  // was [128]
    CHAR outBuffer[16] = {0}; // was [128]

    // send the data in as a 24-bit lump of data -- the driver will know how to deal with it
    RGB_data* rgb = (RGB_data*) malloc(sizeof(RGB_data));
    //rgb->r = rgb->g = rgb->b = 100; // 0x646464
    rgb->r = r;
    rgb->g = g;
    rgb->b = b;
    RtlCopyMemory(inBuffer, rgb, sizeof(RGB_data)); // same as memcpy()
    printf("DEBUG: Printing inBuffer contents:\n");
    for(unsigned int i = 0; i < sizeof(inBuffer); i++)
        printf("%02X ", (unsigned char) inBuffer[i]);
    printf("\n");
    device = CreateFileW(L"\\\\.\\WacomPracticeDeviceLink",
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

    printf("DEBUG: Sending in (buffer contents) 0x%x\n", duration);
    RtlCopyMemory(inBuffer, &duration, sizeof(duration));
    device = CreateFileW(L"\\\\.\\WacomPracticeDeviceLink",
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
