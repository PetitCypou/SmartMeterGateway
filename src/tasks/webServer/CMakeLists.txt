add_library(
	webServer
	webServer.h
	webServer.c
	)
	
target_link_libraries(webServer PRIVATE
        pico_stdlib
        FREERTOS_FILES
        httpServer
    )