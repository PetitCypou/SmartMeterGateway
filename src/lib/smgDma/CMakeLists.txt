add_library(
	smgDma
	smgDma.h
	smgDma.c
	)
	
target_link_libraries(smgDma PRIVATE
    pico_stdlib
    hardware_uart
    hardware_gpio
    hardware_spi
    hardware_dma
    ETHERNET_FILES
    IOLIBRARY_FILES
    LOOPBACK_FILES
    )