add_executable(BuildUSTests 
    unittests.cpp
	catch2.hpp
	catch2.cpp
)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config/config1.buildus
    ${CMAKE_CURRENT_BINARY_DIR}/config/config1.buildus COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config/config2.buildus
    ${CMAKE_CURRENT_BINARY_DIR}/config/config2.buildus COPYONLY)


target_link_libraries(BuildUSTests
    PRIVATE
    FileSystem
)

target_include_directories(BuildUSTests
    PRIVATE
    src
)

