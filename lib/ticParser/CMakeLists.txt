add_library(
	ticParser
	ticParser.h
	ticParser.c
	)

target_link_libraries(ticParser PRIVATE
    pico_stdlib
    hardware_uart
    hardware_gpio
    hardware_spi
    hardware_dma
    ETHERNET_FILES
    IOLIBRARY_FILES
    LOOPBACK_FILES
    )