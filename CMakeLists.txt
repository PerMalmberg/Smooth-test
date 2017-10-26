cmake_minimum_required(VERSION 3.6)
project(SmoothTest)

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

set(IDF_PATH /home/permal/esp/esp-idf)

#add_definitions(-DTEST_QUEUE)
#add_definitions(-DTEST_TIMER)
add_definitions(-DTEST_NETWORK)


include_directories(main
                    components/Smooth/include)

set(SOURCE_FILES
        main/main.cpp
        main/test/test_queue.h
        main/test/test_queue.cpp
        main/test/test_timer.h
        main/test/test_timer.cpp
        main/test/test_network.h
        main/test/test_network.cpp)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})
add_subdirectory(components/Smooth)

target_link_libraries(${PROJECT_NAME} Smooth)

set_target_properties(${PROJECT_NAME}
        PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/out/artifacts
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/out/artifacts
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/out/artifacts)