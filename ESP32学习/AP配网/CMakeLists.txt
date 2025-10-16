idf_component_register(SRCS "WIFI.c" "main.c" "Ws_Server.c"
                    INCLUDE_DIRS ".")

spiffs_create_partition_image(html ../html FLASH_IN_PROJECT)