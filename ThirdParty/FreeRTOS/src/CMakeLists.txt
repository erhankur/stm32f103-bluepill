set(sources
    event_groups.c
    list.c
    queue.c
    tasks.c
    timers.c
    portable/GCC/ARM_CM3/port.c
    portable/MemMang/heap_4.c
)

add_library(freertos ${sources})

# add include directories
target_include_directories(freertos PUBLIC include)
target_include_directories(freertos PRIVATE include/freertos)
target_include_directories(freertos PUBLIC portable/GCC/ARM_CM3)

# set up some defines 

# additional compiler options: use size-optimized version of library in release build, use -O0 in debug build
if(CMAKE_BUILD_TYPE MATCHES Debug)
  set(additional_flags -O0)
else()
  set(additional_flags -Os)
endif()

target_compile_options(freertos PRIVATE ${additional_flags})
