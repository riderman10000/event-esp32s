
// #include <Arduino.h>

#undef EPS      // specreg.h defines EPS which interfere with opencv
#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
// #include "opencv2/imgcodecs.hpp"
#define EPS 192

// defines 
#ifndef MOUNT_POINT
    #define MOUNT_POINT             "/sdcard"
#endif

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

// #include ""
// #include "my_serial.hpp"

extern "C"{
    #include <string.h>
    // #include <sys/unistd.h>
    // #include <sys/stat.h>
    #include "esp_vfs_fat.h"
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
    // SDCardInterface sdcard;
    const char *file_path = MOUNT_POINT"/USER02~1.CSV";
    EventProcessor event_procesor(file_path, 5, 100);
    // Initialize Incremental PCA with 1 component
    // IncrementalPCA ipca(1);
    // cv::Mat data(100, 2, CV_32F);

    // size_t psram_size = esp_spiram_get_size();
    // printf("PSRAM size: %d bytes\n", psram_size);
    // return; 

    printf("XstartX\n");
    cv::Mat reducedData;
    event_procesor.find_start_point();
    float temp_value = 0; 
    float temp_reduced = 0;
    
    cv::Mat test_mean;
    cv::Mat test_eigen_values;
    cv::Mat test_eigen_vectors;

    cv::Mat data(CHUNK_SIZE, 2, CV_32F); // 100 rows, 2 cols, float type
    // cv::PCA pca(data, cv::Mat(), cv::PCA::DATA_AS_ROW, 1); // DATA_AS_ROW for rows as samples

    for(int h = 0; h < 10; h++){
        event_procesor.traverse_events();
        // event_procesor.output_compressed_points();
        // cv::Mat data(CHUNK_SIZE, 2, CV_32F, event_procesor.x_y); // 100 rows, 2 cols, float type
        
        memcpy(data.data, event_procesor.x_y, sizeof(event_procesor.x_y));
        cv::PCA pca(data, cv::Mat(), cv::PCA::DATA_AS_ROW, 1); // DATA_AS_ROW for rows as samples

        if(h > 0){
            pca.mean = test_mean;
            pca.eigenvalues = test_eigen_values;
            pca.eigenvectors = test_eigen_vectors;
        }

        pca.project(data, reducedData); // Reduced to 1 component
        test_mean = pca.mean;
        test_eigen_values = pca.eigenvalues;
        test_eigen_vectors = pca.eigenvectors;

        float alpha = 0.05f;
        
        event_procesor.x_y[0][0] = reducedData.at<float>(0, 0);
        if(h > 0){
            event_procesor.x_y[0][0] = alpha * temp_reduced + (1 - alpha) * temp_value;
        }

        for (int i = 1; i < reducedData.rows; ++i){
            
            event_procesor.x_y[i][0] = alpha * reducedData.at<float>(i, 0) + (1 - alpha) * event_procesor.x_y[i - 1][0] ; 
            if (event_procesor.x_y[i][0] > 50){
                ESP_LOGE(TAG, "ERROR on [%d] reduced data %f, ema: %f\n", i, reducedData.at<float>(i,0), event_procesor.x_y[i][0]);
                sleep(1);
            }
        }

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
        int j = 0;
        // float mean = 0 ;
        int sum = 0;
        for(int i = 0; i< reducedData.rows; i++){
            if((i % mean_chunk_size == 0) && (i != 0)){
                event_procesor.x_y[j][0] = sum / mean_chunk_size;
                sum = 0; 
                j++;    
            }
            sum += event_procesor.x_y[i][0];
        }
        // print the mean data 
        for(int i =0; i < j; i++){
            printf("%f,\n", event_procesor.x_y[i][0]);
            vTaskDelay(5); // yield to the os to reset the watchdog timer 
        }
    }
    printf("XendX\n");
}