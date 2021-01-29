
#=================================
# FileSystem
#=================================
add_library(    FileSystem
                FileSystem/Common.cpp
                FileSystem/ConfigFile.cpp
)

target_include_directories(FileSystem 
   	 PUBLIC 
	${CMAKE_CURRENT_SOURCE_DIR}/FileSystem
)

if (UNIX)
target_link_libraries(FileSystem 
    PUBLIC 
		${Boost_LIBRARIES}
        ${ZLIB_LIBRARIES}
        yaml-cpp
)
else ()
target_link_libraries(FileSystem 
    PUBLIC 
        ${Boost_LIBRARIES}
        yaml-cpp
)
endif ()	
target_include_directories(FileSystem 
    PUBLIC 
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
		${Boost_INCLUDE_DIRS}
)
