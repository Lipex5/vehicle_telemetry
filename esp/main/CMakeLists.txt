set(includes "include")

set(srcs "app_main.c"
         "include/sensors.c"
         "include/wifi.c")

idf_component_register(SRCS "${srcs}"
                    INCLUDE_DIRS "${includes}")