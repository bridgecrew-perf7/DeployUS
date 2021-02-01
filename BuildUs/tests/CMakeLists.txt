add_executable(BuildUSTests 
    unittestscommon.hpp
    YAMLFileTest.cpp
    catch2.hpp
    catch2.cpp
)

file(COPY "config" DESTINATION ".")
file(COPY "prog1" DESTINATION ".")
configure_file("prog1/main.cpp" "prog1/main.cpp" COPYONLY)
configure_file("prog1/utils.cpp" "prog1/utils.cpp" COPYONLY)
configure_file("prog1/utils.hpp" "prog1/utils.hpp" COPYONLY)


target_include_directories(BuildUSTests
    PRIVATE
    src
)

target_link_libraries(BuildUSTests
    PRIVATE
    FileSystem
)
