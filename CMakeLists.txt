cmake_minimum_required(VERSION 2.8)
set(CMAKE_CXX_STANDARD 11)
project( Champagne )
find_package( OpenCV REQUIRED )
find_package(Boost REQUIRED COMPONENTS filesystem system)

include_directories(inc)
add_subdirectory(src)
add_executable(champagne src/main.cpp ${champagne_src_list})
target_link_libraries(champagne ${OpenCV_LIBS} ${Boost_LIBRARIES} )

