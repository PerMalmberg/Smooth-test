cmake_minimum_required(VERSION 3.6)
project(SmoothTest)

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

set(IDF_PATH /home/permal/esp/esp-idf)

#[[ This macro adds an include path to header files found in each component directory,
    i.e. <project directory>/components/<component name>/include
]]
macro(INCLUDE_COMPONENT_HEADERS startDir)
    file(GLOB sub ${startDir} ${startDir}/*)
    foreach (item ${sub})
        if (IS_DIRECTORY ${item})
            include_directories(${item}/include)
        endif ()
    endforeach ()
endmacro()

add_definitions(-DTEST_TIMER)

include_directories(main
        components)


set(SOURCE_FILES
        main/main.cpp
        main/test/test_queue.h
        main/test/test_queue.cpp
        main/test/test_timer.h
        main/test/test_timer.cpp)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})
add_subdirectory(components/Smooth)

target_link_libraries(${PROJECT_NAME} Smooth)

set(COMPONENTS
        freertos
        cxx
        esp32
        driver
        log
        tcpip_adapter
        lwip
        openssl
        soc/esp32
        )

set(COMPONENT_INCLUDE_DIRS "")

foreach (COMP ${COMPONENTS})
    list(APPEND COMPONENT_INCLUDE_DIRS ${IDF_PATH}/components/${COMP}/include)
endforeach ()

include_directories(${COMPONENT_INCLUDE_DIRS})
INCLUDE_COMPONENT_HEADERS(${CMAKE_CURRENT_LIST_DIR}/components)

set_target_properties(${PROJECT_NAME}
        PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/out/artifacts
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/out/artifacts
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/out/artifacts)