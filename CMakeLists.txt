cmake_minimum_required(VERSION 3.6)
project(SmoothTest)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -fsanitize=address -fsanitize=bounds -fsanitize=leak -fsanitize=return -fsanitize=null")

set(IDF_PATH /home/permal/esp/esp-idf)

#add_definitions(-DTEST_HELLO_WORLD)
#add_definitions(-DTEST_WROVER_KIT_RGB_BLINKY)
add_definitions(-DTEST_WROVER_KIT_RGB_BLINKY_JTAG_DEBUGGING)
#add_definitions(-DTEST_QUEUE)
#add_definitions(-DTEST_TIMER)
#add_definitions(-DTEST_NETWORK)
#add_definitions(-DTEST_RGB_LED -DESP_PLATFORM)
#add_definitions(-DTEST_JSON)
#add_definitions(-DTEST_PUBLISH)


include_directories(main
                    components/Smooth/include)

set(SOURCE_FILES
        main/main.cpp
        main/test/test_hello_world.h
        main/test/test_hello_world.cpp
        main/test/test_wrover_kit_blinky.cpp
        main/test/test_wrover_kit_blinky.h
        main/test/test_wrover_kit_blinky_jtag_debugging.cpp
        main/test/test_wrover_kit_blinky_jtag_debugging.h
        main/test/test_queue.h
        main/test/test_queue.cpp
        main/test/test_timer.h
        main/test/test_timer.cpp
        main/test/test_network.h
        main/test/test_network.cpp
        main/test/test_rgb_led.h
        main/test/test_rgb_led.cpp
        main/test/test_json.h
        main/test/test_json.cpp
        main/test/test_publish.h
        main/test/test_publish.cpp)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})
add_subdirectory(components/Smooth)

target_link_libraries(${PROJECT_NAME} Smooth)

set_target_properties(${PROJECT_NAME}
        PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/out/artifacts
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/out/artifacts
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/out/artifacts)