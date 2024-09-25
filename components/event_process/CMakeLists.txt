idf_component_register(
    SRCS "event_process.cpp" "compressor.cpp"
    INCLUDE_DIRS "include"
    REQUIRES opencv my_sd_interface event_lib
)
