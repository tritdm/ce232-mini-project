idf_component_register(SRCS "src/main.c" "src/hdc1080.c" "src/i2c_init.c" "src/oled_string_process.c" "src/oled_task.c"
                    INCLUDE_DIRS "inc")
target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-format")
