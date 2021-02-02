file(COPY "config" DESTINATION ".")
file(COPY "prog1" DESTINATION ".")
configure_file("prog1/main.cpp" "prog1/main.cpp" COPYONLY)
configure_file("prog1/utils.cpp" "prog1/utils.cpp" COPYONLY)
configure_file("prog1/utils.hpp" "prog1/utils.hpp" COPYONLY)

add_executable(BuildUSTests 
    catch2.cpp
    unittestscommon.cpp
    unittests.hpp
    CompilationTest.cpp
    YAMLFileTest.cpp
)

target_link_libraries(BuildUSTests
    PRIVATE
    FileSystem
    GCCDriver
    BuildUSStart
)