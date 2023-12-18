set(TARGET_NAME bootloader)

add_executable(${TARGET_NAME}
        ${TARGET_NAME}.c
        )

set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set_target_properties(${TARGET_NAME} PROPERTIES PICO_TARGET_LINKER_SCRIPT 
${CMAKE_SOURCE_DIR}/src/bootloader/memmap_boot.ld)

pico_add_extra_outputs(${TARGET_NAME})

target_link_libraries(${TARGET_NAME} PRIVATE
        pico_stdlib
    	smgBoot
        )