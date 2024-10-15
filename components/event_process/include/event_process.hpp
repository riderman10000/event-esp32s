#ifndef EVENT_PROCESS_H
#define EVENT_PROCESS_H

#include "opencv2/core.hpp"
#include "my_sd_interface.hpp"
#include "compressor.hpp"
#include "event_lib.hpp"

#ifndef CHUNK_SIZE
    #define CHUNK_SIZE  2000
#endif 


class EventProcessor{
private:
    SDCardInterface sdcard;
    Compressor compressor;

    const char * current_file_path; 

    uint8_t baseline_x = 0, baseline_y = 0;
    uint16_t temp_x = 0, temp_y = 0;
    uint8_t next_x = 0, next_y = 0;
    uint8_t next_next_x = 0, next_next_y = 0;
    bool has_next = false;
    int count = 0;
    int chunk_index= 0 ;
    int start = -1;
    // float x[CHUNK_SIZE], y[CHUNK_SIZE];

    std::pair<uint8_t, uint8_t> distance;
    uint8_t delta = 5;

public:
    // float x_y[CHUNK_SIZE][2] = {0};
    float x[CHUNK_SIZE] = {0};
    float y[CHUNK_SIZE] = {0};

public:
    EventProcessor(char * file_path, uint8_t delta, int count_margin);
    void reset_processor();
    void find_start_point();
    void run();
    esp_err_t traverse_events();
    esp_err_t process_next_point();
    void output_compressed_points();
    int get_current_chunk_index();
    void set_chunk_index(int new_chunk_index);
    long int get_row_traverse_count();

    void list_dir_from_sd(const char* path);
    void close_file();
};

#endif 