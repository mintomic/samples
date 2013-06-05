cmake_minimum_required(VERSION 2.8.5)
set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE INTERNAL "" FORCE)
project(HashTable1)


#------------------------------------------
# Configure the executable and all its dependencies
#------------------------------------------
set(MACOSX_BUNDLE_GUI_IDENTIFIER "com.mycompany.\${PRODUCT_NAME:identifier}")
file(GLOB all_sources *)
add_executable(${PROJECT_NAME} MACOSX_BUNDLE ${all_sources})
include(../../mintomic/cmake/BuildSettings.cmake)

# Libraries
add_subdirectory(../../mintomic/src/mintomic mintomic)
add_subdirectory(../../mintomic/src/mintsystem mintsystem)
add_subdirectory(../../mintomic/src/mintpack mintpack)
include_directories(../../mintomic/include)
add_subdirectory(../../common common)
include_directories(../..)

# Add libraries with dependencies after dependents to satisfy ld linker.
target_link_libraries(${PROJECT_NAME} Common)
target_link_libraries(${PROJECT_NAME} MintPack)
target_link_libraries(${PROJECT_NAME} MintSystem)
get_target_property(TARGET_TYPE Mintomic TYPE)
if(${TARGET_TYPE} STREQUAL "UTILITY")
    # Nothing to link with in the Mintomic subdirectory
else()
    target_link_libraries(${PROJECT_NAME} Mintomic)
endif()
