add_library(
	aegUart
	aegUart.h
	aegUart.c
	)

target_link_libraries(aegUart PRIVATE
    pico_stdlib
    hardware_uart
    hardware_gpio
    hardware_spi
    hardware_dma
    ETHERNET_FILES
    IOLIBRARY_FILES
    LOOPBACK_FILES
    )