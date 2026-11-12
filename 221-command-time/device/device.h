#pragma once

#define DEVICE_NAME "es-cmd-usb"
#define FIRMWARE_VERSION "1.1.0"
#define DEVICE_PROJECT "221-command-time"
#define DEVICE_REPO "https://github.com/c4llery/es-student"

#ifndef DEVICE_BOARD
#define DEVICE_BOARD "unknown"
#endif
#include "pico/unique_id.h"
#include "pico/version.h"
#include <stdio.h>
#include "hardware/regs/addressmap.h"
#include "hardware/regs/sysinfo.h"


static const char *device_project = DEVICE_PROJECT; 
static const char *device_board = DEVICE_BOARD; 
void device_info(void);

void dev_info(void);


struct info_t
{
    uint32_t version;
    char name[13];
    uint8_t revision;
};

extern struct info_t device_card;