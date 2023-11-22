set(TARGET_NAME main)

add_executable(${TARGET_NAME}
        ${TARGET_NAME}.c
        )

        
include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${CMAKE_SOURCE_DIR}/port)
add_subdirectory(lib)
add_subdirectory(tasks)

pico_enable_stdio_usb(${TARGET_NAME} 1)
pico_enable_stdio_uart(${TARGET_NAME} 0)
pico_add_extra_outputs(${TARGET_NAME})


target_link_libraries(${TARGET_NAME} PRIVATE
        pico_stdlib
        hardware_spi
        hardware_dma
        hardware_uart
        ETHERNET_FILES
        FREERTOS_FILES
        IOLIBRARY_FILES
        LOOPBACK_FILES
        TIMER_FILES
        smgDma
        smgNetwork
        smgUart
        ticConstructor
        ticParser
        webServer
        logger
        alarm
        )