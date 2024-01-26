add_library(util_lib util.c)

add_executable(server server.c)
target_link_libraries(server PUBLIC util_lib)

add_executable(client client.c)
target_link_libraries(client PUBLIC util_lib)
