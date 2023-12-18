add_library(
	smgBoot
	smgBoot.h
	smgBoot.c
	)
	
target_link_libraries(smgBoot PRIVATE
    pico_stdlib
    hardware_flash
    hardware_sync
    smgNetwork
    sha1
    )