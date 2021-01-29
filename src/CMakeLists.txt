
#=================================
# Common
#=================================
add_library(    BuildUSCommon
                Common/Common.cpp
)

target_include_directories(BuildUSCommon 
   	 PUBLIC 
	${CMAKE_CURRENT_SOURCE_DIR}/Common
)

target_link_libraries(BuildUSCommon 
    PUBLIC 
		${Boost_LIBRARIES}
)
	
target_include_directories(BuildUSCommon 
    PUBLIC 
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
		${Boost_INCLUDE_DIRS}
)

#=================================
# FileSystem
#=================================
add_library(    FileSystem
                FileSystem/ConfigFile.cpp
)

target_include_directories(FileSystem 
   	 PUBLIC 
	${CMAKE_CURRENT_SOURCE_DIR}/FileSystem
)

target_link_libraries(FileSystem 
    PUBLIC 
		${Boost_LIBRARIES}
        yaml-cpp
)

target_include_directories(FileSystem 
    PUBLIC 
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
		${Boost_INCLUDE_DIRS}
)

