add_executable(hungarian_method ${COMMON_SOURCES} hungarian_method_app.cpp)
target_link_libraries(hungarian_method ${OpenCV_LIBS} ${Boost_LIBRARIES})