add_library(
	alarm
	alarm.h
	alarm.c
	)
	
target_link_libraries(alarm PRIVATE
        pico_stdlib
        FREERTOS_FILES
      	smgDma
        ticParser
        ticConstructor
        smgNetwork
        smgIo
    )