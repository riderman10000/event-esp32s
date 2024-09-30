
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
#include "event_process_ipca.hpp"

extern "C"{
    #include <string.h>
    #include <sys/unistd.h>
    #include <sys/stat.h>
    #include "esp_vfs_fat.h"
    #include "sdmmc_cmd.h"
    #include "driver/sdmmc_host.h"
}


static const char *TAG = "main";

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

    cv::Mat reducedData;
    event_procesor.find_start_point();
    float temp_value = 0;

    for(int h = 0; h < 2; h++){
        event_procesor.traverse_events();
        // event_procesor.output_compressed_points();
        cv::Mat data(CHUNK_SIZE, 2, CV_32F, event_procesor.x_y); // 100 rows, 2 cols, float type
        cv::PCA pca(data, cv::Mat(), cv::PCA::DATA_AS_ROW, 1); // DATA_AS_ROW for rows as samples
        
        pca.project(data, reducedData); // Reduced to 1 component
        
        float alpha = 0.05f;
        
        event_procesor.x_y[0][0] = reducedData.at<float>(0, 0);
        if(h > 0){
            event_procesor.x_y[0][0] = alpha * reducedData.at<float>(0, 0) + (1 - alpha) * temp_value;
        }

        for (int i = 1; i < reducedData.rows; ++i){
            event_procesor.x_y[i][0] = alpha * reducedData.at<float>(i, 0) + (1 - alpha) * event_procesor.x_y[i - 1][0] ; 
        }

        temp_value = event_procesor.x_y[CHUNK_SIZE - 1][0];
    }
    for (int i = 0; i < reducedData.rows; ++i) {
        // printf("%d, %f\n", i, event_procesor.x_y[i][0]);
        printf("%f,\n", event_procesor.x_y[i][0]);
    }

    
    /*
    // for(int i = 0; i < 1; i++){
        event_procesor.traverse_events();
        // event_procesor.output_compressed_points();
        printf("------------------------------------");

        // Step 2: Convert the array to cv::Mat
        cv::Mat data(100, 2, CV_32F, event_procesor.x_y); // 100 rows, 2 cols, float type
        // memcpy(data.data, event_procesor.x_y, sizeof(event_procesor.x_y));

        // ipca.partial_fit(data);

        // Step 3: Perform PCA, keeping only 1 principal component
        cv::PCA pca(data, cv::Mat(), cv::PCA::DATA_AS_ROW, 1); // DATA_AS_ROW for rows as samples
        // Step 4: Project the original data onto the reduced space (1 component)
        cv::Mat reducedData;
        pca.project(data, reducedData); // Reduced to 1 component

        // Output the reduced data (1 principal component per sample)
        printf("Reduced Data (1 component): \n");
        for (int i = 0; i < reducedData.rows; ++i) {
            printf(" %f \n", reducedData.at<float>(i, 0));
        }

    // }

    // // resets the file reading 
    // event_procesor.find_start_point();
    // // read the 100 chunk of data 
    // event_procesor.traverse_events();

    // // after the model is trained, transform the data 
    // // cv::Mat data(100, 2, CV_32F, event_procesor.x_y); // 100 rows, 2 cols, float type
    // memcpy(data.data, event_procesor.x_y, sizeof(event_procesor.x_y));

    // cv::Mat reduced_data = ipca.transform(data);
    // for (int i = 0; i < reduced_data.rows; ++i) {
    //     printf(" %f \n", reduced_data.at<float>(i, 0));
    // }
    // */

}