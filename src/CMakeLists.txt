add_library(logging
    Utility.cpp
    Manager.cpp
    )

configure_file(Version.h.in Version.h)

target_include_directories(
    logging PUBLIC
    "${PROJECT_BINARY_DIR}/../src/"
)

