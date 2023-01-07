set(includes "include")

set(srcs "app_main.c"
         "sensors.c"
         "wifi.c"
         "gps.c"
         "mqtt.c"
         "nmea_parser.c")

idf_component_register(SRCS "${srcs}"
                    INCLUDE_DIRS "${includes}")