add_library(
	ticConstructor
	ticConstructor.h
	ticConstructor.c
	)
	
target_link_libraries(ticConstructor PRIVATE
    ticParser
    pico_stdlib
    hardware_uart
    hardware_gpio
    hardware_spi
    hardware_dma
    ETHERNET_FILES
    IOLIBRARY_FILES
    LOOPBACK_FILES
    )   
         
include_directories(${CMAKE_SOURCE_DIR}/lib)