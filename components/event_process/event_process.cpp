#include "event_process.hpp"

EventProcessor::EventProcessor(char * file_path, uint8_t delta, int count_margin): compressor(delta, count_margin), has_next(false), count(0), chunk_index(0), start(-1) {
        sdcard.set_file_path(file_path);
        current_file_path = file_path;
    }

void EventProcessor::reset_processor(){
    this->has_next = false;
    this->count = 0 ; 
    this->chunk_index = 0;
    this->start = -1; 
    
    this->baseline_x = 0, this->baseline_y = 0;
    this->temp_x = 0, this->temp_y = 0;
    this->next_x = 0, this->next_y = 0;
    this->next_next_x = 0, this->next_next_y = 0;

    sdcard.close_set_file();
    sdcard.set_file_path(current_file_path);
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
    printf("[x] row counter %d, < base x, base y> : <%d, %d> \n", row_counter, baseline_x, baseline_y);
    sdcard.close_set_file();
    // sdcard.set_file_path(MOUNT_POINT"/USER025.CSV");
    sdcard.set_file_path(current_file_path);
}

esp_err_t EventProcessor::traverse_events(){
    while(chunk_index < CHUNK_SIZE){
        if(has_next){
            next_x = next_next_x;
            next_y = next_next_y;
            has_next = false;
        } else {
            if(sdcard.read_row_from_csv(next_x, next_y) == ESP_FAIL){
                return ESP_FAIL;
            }
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
            if(process_next_point() == ESP_FAIL){
                return ESP_FAIL;
            }
        }
    }
    chunk_index = 0; // reset the chunk here... 
    return ESP_OK;
}

esp_err_t EventProcessor::process_next_point(){
    if(sdcard.read_row_from_csv(next_next_x, next_next_y) == ESP_FAIL){
        return ESP_FAIL;
    }
    has_next = true;

    if (count == 1 && is_noise(next_x, next_y, next_next_x, next_next_y, delta)) {
        return ESP_OK; // it was only the return after adding the fail cause of sdcard read failure 
    }

    // compressor.collect_compressed_points(temp_x, temp_y, count, x_y[chunk_index][0], x_y[chunk_index][1]);
    compressor.collect_compressed_points(temp_x, temp_y, count, x[chunk_index], y[chunk_index]);
    chunk_index++;

    baseline_x = next_x;
    baseline_y = next_y;
    temp_x = next_x;
    temp_y = next_y;
    count = 1; 
    return ESP_OK;   
}
void EventProcessor::output_compressed_points(){
    for (int i = 0; i < CHUNK_SIZE; ++i) {
        printf("\n data %d - %f %f ", i+2,  x[i], y[i]);
        // printf("\n data %d - %f %f ", i+2,  x_y[i][0], x_y[i][1]);
    }   
}

int EventProcessor::get_current_chunk_index(){
    return chunk_index;
}

void EventProcessor::set_chunk_index(int new_chunk_index){
    chunk_index = new_chunk_index;
}

long int EventProcessor::get_row_traverse_count(){
    return sdcard.row_counter;
}

void EventProcessor::list_dir_from_sd(const char* path){
    sdcard.list_dir(path);
}