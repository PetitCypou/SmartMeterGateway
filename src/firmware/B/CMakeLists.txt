set(TARGET_NAME firmwareB)

add_executable(${TARGET_NAME}
        ../firmware.c
        )

set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set_target_properties(${TARGET_NAME} PROPERTIES PICO_TARGET_LINKER_SCRIPT
${CMAKE_SOURCE_DIR}/src/firmware/B/memmap_fw_b.ld)

pico_enable_stdio_usb(${TARGET_NAME} 1)
pico_enable_stdio_uart(${TARGET_NAME} 0)
pico_add_extra_outputs(${TARGET_NAME})

target_link_libraries(${TARGET_NAME} PRIVATE
        pico_stdlib
        pico_multicore
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
        smgIo
        ticConstructor
        ticParser
        webServer
        logger
        alarm
        )

        