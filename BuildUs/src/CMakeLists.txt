#=================================
# yaml-cpp
#=================================
execute_process(
    COMMAND bash setup.sh
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
add_subdirectory(yaml-cpp)


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
                FileSystem/BuildUSCache.cpp
)

target_include_directories(FileSystem 
   	 PUBLIC 
	${CMAKE_CURRENT_SOURCE_DIR}/FileSystem
)

target_link_libraries(FileSystem 
    PUBLIC 
		${Boost_LIBRARIES}
        yaml-cpp
        BuildUSCommon
)

target_include_directories(FileSystem 
    PUBLIC 
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
		${Boost_INCLUDE_DIRS}
)

#=================================
# GCCDriver
#=================================
add_library(    GCCDriver
                GCCDriver/GCCDriver.cpp
)

target_include_directories(GCCDriver 
   	 PUBLIC 
	${CMAKE_CURRENT_SOURCE_DIR}/GCCDriver
)

target_link_libraries(GCCDriver
    PUBLIC
    FileSystem
)
