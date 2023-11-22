add_library(
	logger
	logger.h
	logger.c
	)
	
target_link_libraries(logger PRIVATE
        pico_stdlib
        FREERTOS_FILES
        smgDma
        ticParser
        ticConstructor
        smgNetwork
    )