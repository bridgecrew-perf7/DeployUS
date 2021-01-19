add_executable(gittests 
    unittests.hpp
    unittests.cpp
	catch.hpp
)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/testfolder1/letters.txt
    ${CMAKE_CURRENT_BINARY_DIR}/testfolder1/letters.txt COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/testfolder1/numbers.txt
    ${CMAKE_CURRENT_BINARY_DIR}/testfolder1/numbers.txt COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/testfolder1/testfolder2/a.txt
    ${CMAKE_CURRENT_BINARY_DIR}/testfolder1/testfolder2/a.txt COPYONLY)


target_link_libraries(gittests
    PRIVATE
    git
)

target_include_directories(gittests
    PRIVATE
    git
)



