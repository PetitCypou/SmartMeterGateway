add_library(
	sha1
	sha1.h
	sha1.c
	)
	
target_link_libraries(sha1 PRIVATE
    pico_stdlib
    )