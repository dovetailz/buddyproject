set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads REQUIRED)

include_directories(${CMAKE_SOURCE_DIR}/Application/include)
add_executable(Main main.cpp)

target_link_libraries(Main PUBLIC server)
target_link_libraries(Main PUBLIC Threads::Threads)
