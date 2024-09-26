
// #include <Arduino.h>

#undef EPS      // specreg.h defines EPS which interfere with opencv
#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#define EPS 192

// defines 
#ifndef MOUNT_POINT
    #define MOUNT_POINT             "/sdcard"
#endif

#ifndef TOTAL_DATA
    #define TOTAL_DATA              300
#endif

#ifndef CHUNK_SIZE
    #define CHUNK_SIZE              100
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

    event_procesor.find_start_point();
    event_procesor.traverse_events();
    event_procesor.output_compressed_points();
    cv::Mat data(100, 2, CV_32F, event_procesor.x_y); // 100 rows, 2 cols, float type
    cv::PCA pca(data, cv::Mat(), cv::PCA::DATA_AS_ROW, 1); // DATA_AS_ROW for rows as samples
    cv::Mat reducedData;
    pca.project(data, reducedData); // Reduced to 1 component
    for (int i = 0; i < reducedData.rows; ++i) {
        printf(" %f \n", reducedData.at<float>(i, 0));
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