add_library(
        httpServer
        httpParser.c
        httpServer.c
        httpUtil.c
        userHandler.c
        )
        
target_link_libraries(httpServer PRIVATE
    pico_stdlib
    hardware_gpio
    ETHERNET_FILES
    LOOPBACK_FILES
    )