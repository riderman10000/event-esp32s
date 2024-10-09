
// #include <Arduino.h>

#undef EPS      // specreg.h defines EPS which interfere with opencv
#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
// #include "opencv2/imgcodecs.hpp"
#define EPS 192

// defines 
// #ifndef MOUNT_POINT
//     #define MOUNT_POINT             "/sdcard"
// #endif

const char * MOUNT_POINT = "/sdcard"; 

#ifndef TOTAL_DATA
    #define TOTAL_DATA              10000
#endif

#ifndef CHUNK_SIZE
    #define CHUNK_SIZE              2000
#endif

#ifndef EXAMPLE_MAX_CHAR_SIZE
    #define EXAMPLE_MAX_CHAR_SIZE   64
#endif


#include "my_sd_interface.hpp"
#include "event_lib.hpp"
#include "event_process.hpp"
#include "my_mann_kendall.hpp"
#include "my_dtw.hpp"
#include "my_count.hpp"

extern "C"{
    #include <string.h>
    #include "esp_vfs_fat.h"
    #include "esp_timer.h"
    #include "esp_system.h"
    #include "sdmmc_cmd.h"
    #include "driver/sdmmc_host.h"
}

static const char *TAG = "main";
static const int mean_chunk_size = 100;

extern "C"{
    void app_main(void);
}

void app_main(void)
{
    // Start timing
    int64_t start_time = esp_timer_get_time(); // Get start time in microseconds

    // SDCardInterface sdcard;
    // const char *file_path = MOUNT_POINT"/USER02~1.CSV";
    char *file_path = "/sdcard/CLASS4/USER021.CSV";
    EventProcessor event_procesor(file_path, 5, 100);

    // size_t psram_size = esp_spiram_get_size();
    // printf("PSRAM size: %d bytes\n", psram_size);
    // return; 

//     SDCardInterface sdcard;
//     sdcard.list_dir(MOUNT_POINT"/CLASS2");
// }

// /*

    cv::Mat reducedData;
    event_procesor.find_start_point();
    float temp_value = 0; 
    float temp_reduced = 0;
    
    cv::Mat test_mean;
    cv::Mat test_eigen_values;
    cv::Mat test_eigen_vectors;

    cv::Mat data(CHUNK_SIZE, 2, CV_32F); // 100 rows, 2 cols, float type

    // data for the test_data 
    std::vector<double> test_data;

    std::vector<double> temp_average_data_points; 

    // compress by mean 
        int j = 0;
        // float mean = 0 ;
        int sum = 0;

    bool is_end_of_file = false;
    printf("XstartX\n");

    for(int h = 0; !is_end_of_file ; h++){
        vTaskDelay(5);

        if(event_procesor.traverse_events() == ESP_FAIL){
            // break;
            is_end_of_file = true;
            // chunk index 
            
        }
        // event_procesor.output_compressed_points(); // prints the output of the manhattan compressed points... 
        
        memcpy(data.data, event_procesor.x_y, sizeof(event_procesor.x_y));
        cv::PCA pca(data, cv::Mat(), cv::PCA::DATA_AS_ROW, 1); // DATA_AS_ROW for rows as samples

        if(h > 0){ // to update the mean and test_eigen vaules and vectors 
            pca.mean = test_mean;
            pca.eigenvalues = test_eigen_values;
            pca.eigenvectors = test_eigen_vectors;
        }

        // calculate the pca reducing to a single component...
        pca.project(data, reducedData); // Reduced to 1 component
        test_mean = pca.mean;
        test_eigen_values = pca.eigenvalues;
        test_eigen_vectors = pca.eigenvectors;

        // starting the exponential mean averaging 
        float alpha = 0.05f;
        event_procesor.x_y[0][0] = reducedData.at<float>(0, 0);
        if(h > 0){ // to handle the jump from one chunk to another chunk 
            event_procesor.x_y[0][0] = alpha * temp_reduced + (1 - alpha) * temp_value;
        }
        for (int i = 1; i < reducedData.rows; ++i){
            event_procesor.x_y[i][0] = alpha * reducedData.at<float>(i, 0) + (1 - alpha) * event_procesor.x_y[i - 1][0] ; 
            if (event_procesor.x_y[i][0] > 50){
                // to check if we are getting this issue of event_procesor having values greater than 50 
                // well it was due to the watch dog timer which throwed an error and that skipped data... 
                // ESP_LOGE(TAG, "ERROR on [%d] reduced data %f, ema: %f\n", i, reducedData.at<float>(i,0), event_procesor.x_y[i][0]);
                // sleep(1);
                // vTaskDelay(100);
            }
        }
        if(is_end_of_file){
            for(int current_chunk_index = event_procesor.get_current_chunk_index(); 
                current_chunk_index < CHUNK_SIZE; current_chunk_index++){
                event_procesor.x_y[current_chunk_index][0] = event_procesor.x_y[event_procesor.get_current_chunk_index()][0];
                event_procesor.x_y[current_chunk_index][1] = event_procesor.x_y[event_procesor.get_current_chunk_index()][1];
            }
        }

        // assigneing the reduced value for the next chunk calculation 
        temp_value = event_procesor.x_y[CHUNK_SIZE - 1][0];
        temp_reduced = reducedData.at<float>(CHUNK_SIZE - 1, 0);
    
        // print the pca data
        // for (int i = 0; i < reducedData.rows; ++i) {
        //     // printf("%d, %f\n", i, event_procesor.x_y[i][0]);
        //     printf("%f,\n", event_procesor.x_y[i][0]);
        //     // delayMicroseconds(10);
        //     vTaskDelay(5);  // Yield to the OS to reset the watchdog timer
        // }

        // compress by mean 
        j = 0;
        // float mean = 0 ;
        sum = 0;
        for(int i = 0; i< reducedData.rows; i++){
            if(((i+1) % mean_chunk_size == 0) && (i != 0)){
                event_procesor.x_y[j][0] = sum / mean_chunk_size;
                temp_average_data_points.push_back(event_procesor.x_y[j][0]);
                sum = 0; 
                j++;    
            }
            sum += event_procesor.x_y[i][0];
        }
        // print the mean data 
        // for(int i =0; i < j; i++){
        //     printf("%f,\n", event_procesor.x_y[i][0]);
        //     vTaskDelay(5); // yield to the os to reset the watchdog timer 
        // }

        // std::vector<double> test_data;
        // for(int i =0; i < j; i++){
        //     test_data.push_back((double) event_procesor.x_y[i][0]);
        //     printf("test data %d -- %lf \n", i, test_data[i]);
        //     vTaskDelay(5); // yield to the os to reset the watchdog timer 
        // }
        // get_index_of_bottom_and_top_by_mk(test_data);

        for(int i = 0; i < j; i++){
            // printf("%f,\n", event_procesor.x_y[i][0]);
            // test_data.push_back((double) event_procesor.x_y[i][0]);
            
            printf("%f,\n", temp_average_data_points[i]);
            test_data.push_back((double) temp_average_data_points[i]);

            if(is_end_of_file ) { //&& (event_procesor.get_current_chunk_index() < i)){
                // printf("[x]  event current chunk index --- %d --- i -- %d\n", event_procesor.get_current_chunk_index(), i);
                // break;
            }
            vTaskDelay(10);
        }
        temp_average_data_points.clear();

        if(is_end_of_file){
            printf("end of file");
            break;
        }
    }
    printf("XendX\n");
    get_index_of_bottom_and_top_by_mk(test_data);


    // End timing
    int64_t end_time = esp_timer_get_time(); // Get end time in microseconds
    // Calculate elapsed time
    int64_t elapsed_time_us = end_time - start_time;
    // Convert to seconds
    double elapsed_time_s = elapsed_time_us / 1000000.0; // Convert to seconds

    // Print elapsed time
    printf("Elapsed time: %lld microseconds (%.6f seconds)\n", elapsed_time_us, elapsed_time_s);
    printf(" row counter %ld \n", event_procesor.get_row_traverse_count());
    printf("reduced row value %d", test_data.size());
}
// */