/*
 * Description:
 *    A set of c functions designed to be loaded by python to interact with a
 *    windows driver.
 *
 * Author:
 *    Olivier Lesage
 */

#pragma once
#define __USE_MINGW_ANSI_STDIO 1

#include <windows.h>
#include <winioctl.h>
#include <setupapi.h>
#include <initguid.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strsafe.h>

#ifdef __cplusplus
extern "C" {
#endif

BOOL uwire_set_led_color(unsigned char r, unsigned char g, unsigned char b);
BOOL uwire_blink_led(int duration);

#ifdef __cplusplus
}
#endif
