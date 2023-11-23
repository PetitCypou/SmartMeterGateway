add_library(
	smgIo
	smgIo.h
	smgIo.c
	)
	
target_link_libraries(smgIo PRIVATE
    pico_stdlib
    )