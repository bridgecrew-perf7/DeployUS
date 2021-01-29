add_executable(BuildUSTests 
    unittestscommon.hpp
    YAMLFileTest.cpp
    catch2.hpp
    catch2.cpp
)

file(COPY "config" DESTINATION ".")


target_include_directories(BuildUSTests
    PRIVATE
    src
)

target_link_libraries(BuildUSTests
    PRIVATE
    FileSystem
)
