#include "event_process.hpp"

EventProcessor::EventProcessor(const char * file_path, uint8_t delta, int count_margin): compressor(delta, count_margin), has_next(false), count(0), chunk_index(0), start(-1) {
        sdcard.set_file_path(file_path);
    }

void EventProcessor::run(){
    traverse_events();
    output_compressed_points();
}

void EventProcessor::find_start_point(){
    sdcard.read_row_from_csv(baseline_x, baseline_y);
    uint8_t row_counter = 0;
    while (start == -1) {
        sdcard.read_row_from_csv(next_x, next_y);
        distance = compute_distance(baseline_x, baseline_y, next_x, next_y);
        if (compressor.is_within_delta(distance)) {
            start = row_counter;
            break;
        }
        baseline_x = next_x;
        baseline_y = next_y;
        row_counter++;
    }
    sdcard.close_set_file();
    sdcard.set_file_path(MOUNT_POINT"/USER02~1.CSV");
}

void EventProcessor::traverse_events(){
    while(chunk_index < CHUNK_SIZE){
        if(has_next){
            next_x = next_next_x;
            next_y = next_next_y;
            has_next = false;
        } else {
            sdcard.read_row_from_csv(next_x, next_y);
        }
        if (sdcard.row_counter < start) {
            continue;
        }

        distance = compute_distance(baseline_x, baseline_y, next_x, next_y);

        if (compressor.is_within_delta(distance) && count < 100) {
            temp_x += next_x;
            temp_y += next_y;
            count++;
        } else {
            process_next_point();
        }
    }
    chunk_index = 0; // reset the chunk here... 
}

void EventProcessor::modified_traverse_events(int index = 0){

    if(index == 0){
        this->traverse_events();
        return;
    }

    // shifting the last 1000 points to the initial value
    for(int i = 0; i < CHUNK_SIZE/2; i++){
        x_y[i][0] = x_y[(CHUNK_SIZE/2) + i][0] ; 
        x_y[i][1] = x_y[(CHUNK_SIZE/2) + i][1] ;
    }
    chunk_index = 1000;
    while (chunk_index < CHUNK_SIZE){
        if(has_next){
            next_x = next_next_x;
            next_y = next_next_y;
            has_next = false;
        } else {
            sdcard.read_row_from_csv(next_x, next_y);
        }
        if (sdcard.row_counter < start) {
            continue;
        }

        distance = compute_distance(baseline_x, baseline_y, next_x, next_y);

        if (compressor.is_within_delta(distance) && count < 100) {
            temp_x += next_x;
            temp_y += next_y;
            count++;
        } else {
            process_next_point();
        }
    }
    chunk_index = 0; // reset the chunk here... 
}

void EventProcessor::process_next_point(){
    sdcard.read_row_from_csv(next_next_x, next_next_y);
    has_next = true;

    if (count == 1 && is_noise(next_x, next_y, next_next_x, next_next_y, delta)) {
        return;
    }

    compressor.collect_compressed_points(temp_x, temp_y, count, x_y[chunk_index][0], x_y[chunk_index][1]);
    chunk_index++;

    baseline_x = next_x;
    baseline_y = next_y;
    temp_x = next_x;
    temp_y = next_y;
    count = 1;    
}
void EventProcessor::output_compressed_points(){
    for (int i = 0; i < CHUNK_SIZE; ++i) {
        printf("\n data %d - %f %f ", i+2,  x_y[i][0], x_y[i][1]);
    }   
}