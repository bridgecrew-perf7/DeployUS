add_library(git
    argParser.h 
    argParser.cpp
    commands/BaseCommand.hpp
    commands/BaseCommand.cpp
    commands/HelpCommand.hpp
    commands/HelpCommand.cpp
    commands/InitCommand.hpp
    commands/InitCommand.cpp
    commands/AddCommand.hpp
    commands/AddCommand.cpp)

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
