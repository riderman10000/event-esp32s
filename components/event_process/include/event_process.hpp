#ifndef EVENT_PROCESS_H
#define EVENT_PROCESS_H

#include "opencv2/core.hpp"
#include "my_sd_interface.hpp"
#include "compressor.hpp"
#include "event_lib.hpp"

#ifndef CHUNK_SIZE
    #define CHUNK_SIZE  100
#endif 


class EventProcessor{
private:
    SDCardInterface sdcard;
    Compressor compressor;

    uint8_t baseline_x = 0, baseline_y = 0;
    uint16_t temp_x = 0, temp_y = 0;
    uint8_t next_x = 0, next_y = 0;
    uint8_t next_next_x = 0, next_next_y = 0;
    bool has_next = false;
    int count = 0;
    int chunk_index= 0 ;
    int start = -1;
    float x[CHUNK_SIZE], y[CHUNK_SIZE];

    std::pair<uint8_t, uint8_t> distance;
    uint8_t delta = 5;

public:
    float x_y[CHUNK_SIZE][2] = {0};

public:
    EventProcessor(const char * file_path, uint8_t delta, int count_margin);
    void find_start_point();
    void run();
    void traverse_events();
    void process_next_point();
    void output_compressed_points();
};

#endif 