include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/5376968f6948923e2411081fd9372e71a59d8e77.zip
)

FetchContent_MakeAvailable(googletest)
add_library(GTest::GTest INTERFACE IMPORTED)
target_link_libraries(GTest::GTest INTERFACE gtest_main)

add_executable(test_logging
    Test_Event.cpp
    Test_RingBuffer.cpp
    Test_Factory.cpp
    Test_GenericEvent.cpp
    Test_Manager.cpp
    )

target_link_libraries(test_logging
    PRIVATE
        GTest::GTest
        logging
)

target_include_directories(test_logging PUBLIC "${PROJECT_BINARY_DIR}/../src")

add_test(NAME example_test COMMAND test_logging)