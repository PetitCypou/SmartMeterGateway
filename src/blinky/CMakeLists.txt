set(TARGET_NAME blinkyA)
add_executable(${TARGET_NAME}
        blinky.c
        )

set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set_target_properties(${TARGET_NAME} PROPERTIES PICO_TARGET_LINKER_SCRIPT 
${CMAKE_SOURCE_DIR}/src/blinky/memmap_blinky_a.ld)

pico_add_extra_outputs(${TARGET_NAME})

target_link_libraries(${TARGET_NAME} PRIVATE
        pico_stdlib
        )
        
        
set(TARGET_NAME blinkyB)

add_executable(${TARGET_NAME}
        blinky.c
        )

set_target_properties(${TARGET_NAME} PROPERTIES PICO_TARGET_LINKER_SCRIPT 
${CMAKE_SOURCE_DIR}/src/blinky/memmap_blinky_b.ld)

pico_add_extra_outputs(${TARGET_NAME})

target_link_libraries(${TARGET_NAME} PRIVATE
        pico_stdlib
        )
        