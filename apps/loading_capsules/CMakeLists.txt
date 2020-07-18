add_executable(loading_capsules ${COMMON_SOURCES} loading_capsules_app.cpp)
target_link_libraries(loading_capsules ${OpenCV_LIBS} ${Boost_LIBRARIES})