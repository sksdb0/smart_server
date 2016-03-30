add_subdirectory(db)

add_executable(asio_chat_client client.cc)
target_link_libraries(asio_chat_client muduo_net)

add_executable(asio_chat_client1 client1.cc)
target_link_libraries(asio_chat_client1 muduo_net)

add_executable(asio_chat_appclient appclient.cc)
target_link_libraries(asio_chat_appclient muduo_net)

add_executable(asio_chat_server server.cc device_manager.cc)
target_link_libraries(asio_chat_server muduo_net dbaccess)
