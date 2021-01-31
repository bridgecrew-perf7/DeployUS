add_library(git
    common.hpp
    common.cpp
    filesystem/GitFilesystem.hpp
    filesystem/GitFilesystem.cpp
    filesystem/GitIndexFile.hpp
    filesystem/GitIndexFile.cpp
    objects/BaseGitObject.hpp
    objects/BaseGitObject.cpp
    objects/GitBlob.hpp
    objects/GitBlob.cpp
    objects/GitTree.hpp
    objects/GitTree.cpp
    objects/GitCommit.hpp
    objects/GitCommit.cpp
    commands/InitCommand.hpp
    commands/InitCommand.cpp
    commands/AddCommand.hpp
    commands/AddCommand.cpp
    commands/CommitCommand.hpp
    commands/CommitCommand.cpp
    commands/CheckoutCommand.hpp
    commands/CheckoutCommand.cpp
    start.hpp
    start.cpp)

if (UNIX)
target_link_libraries(git 
    PUBLIC 
		${Boost_LIBRARIES}
		${ZLIB_LIBRARIES}
)
else ()
target_link_libraries(git 
    PUBLIC 
		${Boost_LIBRARIES}
)
endif ()	
target_include_directories(git 
    PUBLIC 
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
		${Boost_INCLUDE_DIRS}
)
