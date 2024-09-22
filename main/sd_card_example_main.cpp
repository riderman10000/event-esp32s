#include <Arduino.h>
#include "my_sd_interface.h"

extern "C"{
    #include <string.h>
    #include <sys/unistd.h>
    #include <sys/stat.h>
    #include "esp_vfs_fat.h"
    #include "sdmmc_cmd.h"
    #include "driver/sdmmc_host.h"
}

#define EXAMPLE_MAX_CHAR_SIZE   64
#define MOUNT_POINT             "/sdcard"

static const char *TAG = "example";

extern "C"{
    void app_main(void);
}

void app_main(void)
{
    SDCardInterface sdcard;

    uint8_t x[10] = {0};
    uint8_t y[10] = {0};

    // Use POSIX and C standard library functions to work with files:
    const char *file_path = MOUNT_POINT"/USER02~1.CSV";
    sdcard.set_file_path(file_path);
    for(int i = 0; i < 10; i++){
        sdcard.read_row_from_csv(x[i], y[i]);
    }

    for(int i = 0; i < 10 ; i++){
        printf("data - %d %d \n", x[i], y[i]);
    }

}
