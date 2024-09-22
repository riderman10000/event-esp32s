#ifndef MY_SD_INTERFACE_H
#define MY_SD_INTERFACE_H

#include <Arduino.h>

#include <stdio.h>
#include <string.h>
#include <sstream>
#include <sys/unistd.h>
#include <sys/stat.h>
#include <dirent.h>

#include "esp_log.h"
#include "esp_err.h"
#include "esp_vfs_fat.h"

#include "sdmmc_cmd.h"
#include "driver/sdmmc_host.h"
#include "vector"

# ifndef MOUNT_POINT
    #define MOUNT_POINT             "/sdcard"
#endif 

#ifndef EXAMPLE_MAX_CHAR_SIZE
    #define EXAMPLE_MAX_CHAR_SIZE    64
#endif

static const char *SD_TAG = "SD TAG";
static const char mount_point[] = MOUNT_POINT;

std::vector<std::string> split(const std::string &line, char delimiter);


class SDCardInterface{
private:
    FILE *file;
    sdmmc_card_t * card;
    // char *mount_point; 
    
public:
    SDCardInterface(void);
    void list_dir(const char* path);
    esp_err_t s_example_write_file(const char *path, char *data);
    esp_err_t s_example_read_file(const char *path);
    esp_err_t read_from_csv(const char *filename);
    
    // these are to read csv files
    esp_err_t set_file_path(const char *filename, bool skip_header = true);
    esp_err_t read_row_from_csv();
    esp_err_t read_row_from_csv(uint8_t &x, uint8_t &y);
    ~SDCardInterface();
};

#endif 