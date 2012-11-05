env = Environment(CCFLAGS='-O0 -g -Wall -Wextra',
                  CPPPATH='src',
                  LIBS=['glog', 'pthread'])

netlib_src = Split("""
src/bloom_filter.cpp
src/epoll_socket_handler.cpp
src/event_loop.cpp
src/event_socket_server.cpp
src/file_io.cpp
src/hash.cpp
src/net.cpp
src/request_handler.cpp
src/socket_client.cpp
src/socket_io.cpp
src/socket_server.cpp
src/thread_pool_socket_server.cpp
src/simple_socket_server.cpp
src/buffer_io.cpp
""")

env.Library('netlib', netlib_src)

env.Program('bloom_filter_test', ['tests/bloom_filter_test.cpp', 'libnetlib.a'])
env.Program('event_sock_server_test', ['tests/event_sock_server_test.cpp', 'libnetlib.a'])
env.Program('event_test', ['tests/event_test.cpp', 'libnetlib.a'])
env.Program('file_io_test', ['tests/file_io_test.cpp', 'libnetlib.a'])
env.Program('sock_client_test', ['tests/sock_client_test.cpp', 'libnetlib.a'])
env.Program('thread_pool_test', ['tests/thread_pool_test.cpp', 'libnetlib.a'])
env.Program('thread_test', ['tests/thread_test.cpp', 'libnetlib.a'])
env.Program('tp_socket_server_test', ['tests/tp_socket_server_test.cpp', 'libnetlib.a'])
env.Program('simple_server_test', ['tests/simple_socket_server_test.cpp', 'libnetlib.a'])
env.Program('uf_test', ['tests/uf_test.cpp', 'libnetlib.a'])
env.Program('buffer_io_test', ['tests/buffer_io_test.cpp', 'libnetlib.a'])
