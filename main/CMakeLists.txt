idf_component_register(SRCS "state.c" "main.c" "wlan.c" "mqttcomm.c" "led_status.c" "status_broadcaster.c" "analog_reader.c"
                    INCLUDE_DIRS ".")
nvs_create_partition_image(nvs ../nvs.csv FLASH_IN_PROJECT)
