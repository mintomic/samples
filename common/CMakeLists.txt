project(Common)

include_directories(.)
include_directories(../mintomic/include)

file(GLOB INCFILES *.h)
file(GLOB SRCFILES *.c *.cpp)
add_library(Common ${SRCFILES} ${INCFILES})
include(../mintomic/cmake/BuildSettings.cmake)
