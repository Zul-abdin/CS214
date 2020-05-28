cmake_minimum_required(VERSION 3.15)
project(Assignment3 C)

set(CMAKE_C_STANDARD 90)

add_executable(Assignment3 server.c client.c)
add_executable(server server.c)
add_executable(client client.c)

include_directories(/usr/include/openssl)
target_link_libraries(server /usr/lib/x86_64-linux-gnu/libssl.so)
target_link_libraries(client /usr/lib/x86_64-linux-gnu/libssl.so)
target_link_libraries(server /usr/lib/x86_64-linux-gnu/libcrypto.so)
target_link_libraries(client /usr/lib/x86_64-linux-gnu/libcrypto.so)
target_link_libraries(server /usr/lib/x86_64-linux-gnu/libpthread.so)