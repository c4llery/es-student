#define DEVICE_NAME "es-cmd-usb"
#define FIRMWARE_VERSION "1.0.0"
#define DEVICE_PROJECT "211-command-usb"
#define DEVICE_REPO "https://github.com/c4llery/es-student"

#ifndef DEVICE_BOARD
#define DEVICE_BOARD "unknown"
#endif

struct info_t;

static const char *device_project = DEVICE_PROJECT; 
static const char *device_board = DEVICE_BOARD; 
void device_info(void);

void dev_info(void);