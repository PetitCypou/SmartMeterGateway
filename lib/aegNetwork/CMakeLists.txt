add_library(
	aegNetwork
	aegNetwork.h
	aegNetwork.c
	)
	
target_link_libraries(aegNetwork PRIVATE
        pico_stdlib
        hardware_spi
        hardware_dma
        ETHERNET_FILES
        IOLIBRARY_FILES
        LOOPBACK_FILES
        )