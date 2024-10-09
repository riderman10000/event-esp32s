#include "my_sd_interface.hpp"

static const char *SD_TAG = "SD TAG";

void func(void){

}

// Function to split a line by a delimiter (comma in this case)
std::vector<std::string> split(const std::string &line, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream ss(line);
    
    // Extract tokens separated by the delimiter
    while (std::getline(ss, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

// class methods definition
SDCardInterface::SDCardInterface(void){
    esp_err_t ret;
    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = false, 
        .max_files = 5,
        .allocation_unit_size = 16 * 1024
    };

    ESP_LOGI(SD_TAG, "Initializing SD card");
    // use settings defined above to initialize SD card and mount FAT filesystem.
    // note: esp_vfs_fat_sdmmc/sdspi_mount is all-in-one convenience functions
    ESP_LOGI(SD_TAG, "Using SDMMC peripheral");

    // By default, SD card frequency is initialized to SDMMC_FREQ_DEFAULT(20MHz)
    sdmmc_host_t host = SDMMC_HOST_DEFAULT();

    // This initializes the slot without card detect (CD) and write protect (WP) signals.
    // Modify slot_config.gpio_cs and slot_config.gpio_wp if your board has these signals.
    sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT();

    // set bus width to use 
    slot_config.width = 1;

    // on chips where the GPIOs used for SD card can be configured, set them in 
    // the slot_config structure:
    slot_config.cmd = GPIO_NUM_38;
    slot_config.clk = GPIO_NUM_39;
    slot_config.d0 = GPIO_NUM_40;

    // slot_config.flags |= SDMMC_SLOT_FLAG_INTERNAL_PULLUP;

    // mounting file system
    ESP_LOGI(SD_TAG, "Mounting filesystem");
    ret = esp_vfs_fat_sdmmc_mount(mount_point, &host, &slot_config, &mount_config, &this->card);

    if(ret != ESP_OK){
        if(ret == ESP_FAIL){
            ESP_LOGE(SD_TAG, "Failed to mount filesystem."
                "If you want the card to be formatted, set the EXAMPLE_FORMAT_IF_MOUNT_FAILED menuconfig option.");
        } else {
            ESP_LOGE(SD_TAG, "Failed to initialize the card (%s). "
                     "Make sure SD card lines have pull-up resistors in place.", esp_err_to_name(ret));
        }
        return;
    }
    ESP_LOGI(SD_TAG, "Filesystem mounted");

    // card has been initialized, print its properties 
    sdmmc_card_print_info(stdout, card);
    // list the directory and file in the card. 
    list_dir(MOUNT_POINT);
}

void SDCardInterface::list_dir(const char * path){
    DIR* dir = opendir(path);
    if (dir == NULL){
        ESP_LOGE(SD_TAG, "Failed to open directory: %s", path);
        return;
    }

    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL){
        // print the name of each file/directory found 
        ESP_LOGI(SD_TAG, "Found: %s", entry->d_name);
    }
    closedir(dir); // close the directory when done 
}

esp_err_t SDCardInterface::s_example_write_file(const char *path, char *data){
    ESP_LOGI(SD_TAG, "Opening file %s", path);
    FILE *f = fopen(path, "w");
    if (f == NULL) {
        ESP_LOGE(SD_TAG, "Failed to open file for writing");
        return ESP_FAIL;
    }
    fprintf(f, data);
    fclose(f);
    ESP_LOGI(SD_TAG, "File written");

    return ESP_OK;
}

esp_err_t SDCardInterface::s_example_read_file(const char *path)
{
    ESP_LOGI(SD_TAG, "Reading file %s", path);
    FILE *f = fopen(path, "r");
    if (f == NULL) {
        ESP_LOGE(SD_TAG, "Failed to open file for reading");
        return ESP_FAIL;
    }
    char line[EXAMPLE_MAX_CHAR_SIZE];
    fgets(line, sizeof(line), f);
    fclose(f);

    // strip newline
    char *pos = strchr(line, '\n');
    if (pos) {
        *pos = '\0';
    }
    ESP_LOGI(SD_TAG, "Read from file: '%s'", line);

    return ESP_OK;
}

esp_err_t SDCardInterface::read_from_csv(const char *filename){
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        ESP_LOGE(SD_TAG, "Failed to open file: %s", filename);
        return ESP_FAIL;
    }

    char line[256];
    // while (fgets(line, sizeof(line), file)) {
    for(int i =0; i < 10; i++){
        fgets(line, sizeof(line), file);
        // Convert char array to C++ string
        std::string strLine(line);
        // Split the line by commas
        std::vector<std::string> tokens = split(strLine, ',');

        // Print each token from the CSV line
        for (std::vector<std::string>::iterator it = tokens.begin(); it != tokens.end(); ++it) {
            printf("%s ", (*it).c_str());
        }
        printf("\n");  // Newline after each row
    }

    fclose(file);  // Close the file after reading
    return ESP_OK;
}

// read csv files 
esp_err_t SDCardInterface::set_file_path(const char *filename, bool skip_header){
    this->row_counter = -1;
    this->file = fopen(filename, "r");
    if(file == NULL){
        ESP_LOGE(SD_TAG, "failed to open file: %s", filename);
        return ESP_FAIL;
    }
    if(skip_header){
        this->read_row_from_csv();
        ESP_LOGI(SD_TAG, "Skipped the header row");
    }
    return ESP_OK;
}

esp_err_t SDCardInterface::read_row_from_csv(){
    char line[256];
    fgets(line, sizeof(line), this->file);
    // convert char array to c++ string 
    std::string strLine(line);
    // split the line by commas 
    std::vector<std::string> tokens = split(strLine, ',');

    // print each token from the CSV line 
    for(std::vector<std::string>::iterator it = tokens.begin(); it != tokens.end(); ++it){
        
        // printf("s - %s ", (*it).c_str());
        int test = atoi((*it).c_str());
        // printf("f - %d ", test);
    }
    // printf("\n");
    this->row_counter++;
    return ESP_OK;
}  

esp_err_t SDCardInterface::read_row_from_csv(uint8_t &x, uint8_t &y){
    char line[256];
    
    if(fgets(line, sizeof(line), this->file) == NULL){
        // printf("End of File Handle With Care");
        return ESP_FAIL;
    }
    // convert char array to c++ string 
    std::string strLine(line);
    // split the line by commas 
    std::vector<std::string> tokens = split(strLine, ',');

    // print each token from the CSV line 
    int column = 0; 
    for(std::vector<std::string>::iterator it = tokens.begin(); it != tokens.end(); ++it){
        uint8_t test = atoi((*it).c_str());
        switch (column){
        // assign value to the column
            case 1: // column second x 
                x = test; 
                break;
            case 2: // column third y 
                y = test;
                break;
        }
        column++;
    }
    row_counter++;
    return ESP_OK;
}  

esp_err_t SDCardInterface::close_set_file(){
    try{
        fclose(this->file);
        return ESP_OK;
    }
    catch(int error_code){
        ESP_LOGE(SD_TAG, "Error On closing file, error code %d", error_code);
        return ESP_FAIL;
    }
}

esp_err_t SDCardInterface::read_next_10_row_from_csv(){
    uint8_t current_x, current_y; 
    for(int i = 0; i < 10; i++){
        try{
            this->read_row_from_csv(current_x, current_y);
        }catch(int num){
            ESP_LOGE(SD_TAG, "failed to read from file");
            return ESP_FAIL;
        }
        printf("index: %d, x: %d, y: %d \n", i, current_x, current_y);
    }
    return ESP_OK;
}

SDCardInterface::~SDCardInterface(){
    // all done, umount partition and disable SDMMC peripheral 
    // esp_vfs_fat_sdcard_format(mount_point, this->card);
    try{
        fclose(this->file);
    }
    catch(int error_code){
        ESP_LOGE(SD_TAG, "Error On closing file, error code %d", error_code);
    }
    ESP_LOGI(SD_TAG, "card unmounted");
}