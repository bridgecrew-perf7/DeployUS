file(COPY "config" DESTINATION ".")
file(COPY "prog1" DESTINATION ".")
file(COPY "prog2" DESTINATION ".")
file(COPY "prog_compileissue1" DESTINATION ".")
file(COPY "prog_compileissue2" DESTINATION ".")
file(COPY "prog_linkerissue1" DESTINATION ".")
file(COPY "prog_linkerissue2" DESTINATION ".")

add_executable(BuildUSTests 
    catch2.cpp
    unittestscommon.cpp
    unittests.hpp
    CleanTest.cpp
    ConfigTest.cpp
    CompilationTest.cpp
    LinkingTest.cpp
)

target_link_libraries(BuildUSTests
    PRIVATE
    FileSystem
    GCCDriver
    BuildUSStart
)