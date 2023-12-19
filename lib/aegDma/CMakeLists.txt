add_library(
	aegDma
	aegDma.h
	aegDma.c
	)
	
target_link_libraries(aegDma PRIVATE
    pico_stdlib
    hardware_uart
    hardware_gpio
    hardware_spi
    hardware_dma
    ETHERNET_FILES
    IOLIBRARY_FILES
    LOOPBACK_FILES
    )