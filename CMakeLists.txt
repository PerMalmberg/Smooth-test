cmake_minimum_required(VERSION 3.6)
project(esp32_test)

set(CMAKE_CXX_STANDARD 14)

set( IDF_PATH /home/permal/esp/esp-idf)

#[[ This macro adds an include path to header files found in each component directory,
    i.e. <project directory>/components/<component name>/include
]]
macro(INCLUDE_COMPONENT_HEADERS startDir )
    file(GLOB sub ${startDir} ${startDir}/*)
    foreach(item ${sub})
        if( IS_DIRECTORY ${item})
            include_directories( ${item}/include )
        endif()
    endforeach()
endmacro()


set(SOURCE_FILES
        main/main.cpp
        )

add_executable(esp32_test ${SOURCE_FILES})

set( COMPONENTS
        freertos
        cxx
        esp32
        driver
        log
        tcpip_adapter
        lwip
        )

set( COMPONENT_INCLUDE_DIRS "" )

foreach (COMP ${COMPONENTS})
    list( APPEND COMPONENT_INCLUDE_DIRS ${IDF_PATH}/components/${COMP}/include )
endforeach ()

include_directories( ${COMPONENT_INCLUDE_DIRS} )
INCLUDE_COMPONENT_HEADERS(${CMAKE_CURRENT_LIST_DIR}/components)

