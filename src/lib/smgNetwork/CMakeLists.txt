add_library(
	smgNetwork
	smgNetwork.h
	smgNetwork.c
	)
	
target_link_libraries(smgNetwork PRIVATE
        pico_stdlib
        hardware_spi
        hardware_dma
        ETHERNET_FILES
        IOLIBRARY_FILES
        LOOPBACK_FILES
        DHCP4_FILES
        DHCP6_FILES
        TIMER_FILES
        DNS_FILES
        )
        
include_directories(${CMAKE_SOURCE_DIR}/port)