#include <Arduino.h>
#include "my_sd_interface.h"
#include "event_lib.h"

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

    uint8_t start = -1 ;
    uint8_t x_prev;
    uint8_t y_prev;
    uint8_t current_x, current_y;
    uint8_t temp_x, temp_y;
    uint8_t delta = 5; 
    std::pair<uint8_t, uint8_t> distance;
    std::vector<std::pair<uint8_t, uint8_t>> current_chunk; // chunk to store point 

    int count = 0;
    int count_margin = 100; 
    int chunk_size = 1000;

    sdcard.read_row_from_csv(x_prev, y_prev);
    uint8_t counter = 1;
    while(start == -1){
        sdcard.read_row_from_csv(current_x, current_y);
        distance = compute_distance(x_prev, y_prev, current_x, current_y);
        if (distance.first <= delta && distance.second <= delta){
            start = counter;
        }
        x_prev = current_x;
        y_prev = current_y;
        counter ++; 
    }

    // traverse through each event
    for(int i = 0; i < 10; i++){
        sdcard.read_row_from_csv(current_x, current_y);
        distance = compute_distance(x_prev, y_prev, current_x, current_y);
        
        // check if the point is within the threshold 
        if (distance.first <= delta && distance.second <= delta && count < count_margin){
            temp_x += current_x;
            temp_y += current_y;

        } else {
            // check if we've reached the las event 
            if(counter + 1 >= chunk_size){
                if(count > 1) {
                    collect_compressed_points(temp_x, temp_y, count, current_chunk);
                }
                break;
            } 
            if(count == 1 && is_noise(x_prev, y_prev, current_x, current_y, delta)){
                continue;
            }

            // collect the average point 
            collect_compressed_points(temp_x, temp_y, count, current_chunk); 

            // check if the current chunk is full 

        }
        x_prev = current_x;
        y_prev = current_y;
        counter++;
    }

    for(int i = 0; i < 10 ; i++){
        printf("data - %d %d \n", x[i], y[i]);
    }

}
