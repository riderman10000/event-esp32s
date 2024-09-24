#ifndef EVENT_PROCESS_H
#define EVENT_PROCESS_H


#include "opencv2/core.hpp"
#include "my_sd_interface.hpp"

#define CHUNK_SIZE  100

void test_opencv(void);

class EventProcessor{
private:
    SDCardInterface sdcard;    


    uint8_t baseline_x, baseline_y;
    uint16_t temp_x, temp_y;
    uint8_t next_x, next_y;
    uint8_t next_next_x, next_next_y;
    bool has_next;
    int count;
    int chunk_index;
    int start;
    float x[CHUNK_SIZE], y[CHUNK_SIZE];

public:
    EventProcessor(const std::string &file_path, uint8_t delta, int count_margin);



};

#endif 