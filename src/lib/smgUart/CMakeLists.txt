add_library(
	smgUart
	smgUart.h
	smgUart.c
	)

target_link_libraries(smgUart PRIVATE
    pico_stdlib
    hardware_uart
    hardware_gpio
    hardware_spi
    hardware_dma
    ETHERNET_FILES
    IOLIBRARY_FILES
    LOOPBACK_FILES
    )