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

#define MOUNT_POINT             "/sdcard"
#define TOTAL_DATA              300
#define CHUNK_SIZE              50
#define EXAMPLE_MAX_CHAR_SIZE   64

static const char *TAG = "main";

extern "C"{
    void app_main(void);
}

void app_main(void)
{
    SDCardInterface sdcard;

    // Use POSIX and C standard library functions to work with files:
    const char *file_path = MOUNT_POINT"/USER02~1.CSV";
    sdcard.set_file_path(file_path);

    float x[CHUNK_SIZE] = {0};
    float y[CHUNK_SIZE] = {0};

    uint8_t current_x = 0, current_y = 0;
    uint8_t x_prev = 0, y_prev = 0;
    uint16_t temp_x = 0, temp_y = 0;
    uint8_t next_x = 0, next_y = 0;
    bool has_next = false; 
    
    uint8_t delta = 5; 
    int count_margin = 100; 
    int chunk_size = 200;
    int count = 0, chunk_index = 0; 
    
    std::pair<uint8_t, uint8_t> distance;

    // find start point 
    int start = -1 ;
    sdcard.read_row_from_csv(x_prev, y_prev);
    uint8_t row_counter = 1;
    while(start == -1){
        sdcard.read_row_from_csv(current_x, current_y);
        distance = compute_distance(x_prev, y_prev, current_x, current_y);
        if (distance.first <= delta && distance.second <= delta){
            start = row_counter;
            break; // why removed this?
        }
        x_prev = current_x;
        y_prev = current_y;
        row_counter ++; 
    }

    // traverse through each event
    // for(int i = 0; chunk_index < 10; i++, row_counter++){
    while(chunk_index < CHUNK_SIZE){
        // read the current row, either form 'next' or new
        if(has_next){
            current_x = next_x;
            current_y = next_y;
            has_next = false;
        } else{
            sdcard.read_row_from_csv(current_x, current_y);
        }

        // calculate manhattan distance from previous point 
        printf("traverse i: %d | xp %d, yp %d, x %d y %d\n", row_counter, x_prev, y_prev, current_x, current_y);
        distance = compute_distance(x_prev, y_prev, current_x, current_y);
        
        // within delta and under margin? accumulate points 
        printf("\t distance %d, %d \n", distance.first, distance.second);
        if (distance.first <= delta && distance.second <= delta && count < count_margin){
            temp_x += current_x;
            temp_y += current_y;
            count++;
        } else {
            // check if this is the last point in the chunk 
            if(row_counter + 1 >= chunk_size){
                if(count > 1) {
                    printf("\tinside the traverse\n");
                    collect_compressed_points(temp_x, temp_y, count, x[chunk_index], y[chunk_index]);
                    // chunk_index ++;
                }
                break; // exit after processing the final point 
            } 
            // check if current point is noise 
            sdcard.read_row_from_csv(next_x, next_y); 
            if(count == 1 && is_noise(current_x, current_y, next_x, next_y, delta)){
                has_next = true;
                continue; // skip this noisy point 
            }

            // write compressed points and reset counters 
            collect_compressed_points(temp_x, temp_y, count, x[chunk_index], y[chunk_index]); 
            printf("test test %f %f ", x[chunk_index], y[chunk_index]);

            // check if the current chunk is full 
            chunk_index++; 
            if(row_counter >= chunk_size){
                ESP_LOGE(TAG, "row_counter %d, and chunk size %d\n", row_counter, chunk_size);
                row_counter = 0; 
            }
            
            // set baseline for next accumulation 
            x_prev = current_x;
            y_prev = current_y;

            temp_x = current_x;
            temp_y = current_y;

            count = 1;
        }
        printf("\t low delta tempx %d tempy %d count %d ", temp_x, temp_y, count );
        row_counter++;
    }

    // output compressed points
    for(int i = (0+2); i < (20+2) ; i++){
        printf("\n %d data - %f %f %d ", i, x[i-2], y[i-2], sizeof(int));
    }

}