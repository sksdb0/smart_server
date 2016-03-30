set(CMAKE_LIBRARY_PATH /usr/local/lib)
add_library(dbaccess dbaccess.cc dbmanager.cc)
target_link_libraries(dbaccess mysqlclient)
