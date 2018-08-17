//
// Created by null on 18-8-15.
//

#ifndef BOXED_HTTP_SERVICE_H
#define BOXED_HTTP_SERVICE_H
#include "core/distributed.hh"
#include "core/stream.hh"
#include "core/sleep.hh"
#include <iostream>

class http_service {
    const unsigned int _port;
    seastar::future<const unsigned int> uu(){return seastar::make_ready_future<const unsigned int>(_port);}
public:
    http_service(unsigned int port):_port(port){}
    void run(){
    }
    seastar::future<> stop(){return seastar::make_ready_future();}
};

extern seastar::distributed<http_service> _g_http_service;


struct connection {
    seastar::connected_socket _socket;
    seastar::socket_address _addr;
    seastar::input_stream<char> _in;
    seastar::output_stream<char> _out;
    connection(seastar::connected_socket&& socket, seastar::socket_address addr)
            : _socket(std::move(socket))
            , _addr(addr)
            , _in(_socket.input())
            , _out(_socket.output())
    {}
    ~connection() {}
};

constexpr
auto listen_proc=[](uint16_t _port){
    return seastar::do_with(seastar::engine().listen(seastar::make_ipv4_address({_port}),{true}),
                            [](auto& listener){
                                return seastar::keep_doing([&listener]{
                                    std::cout<<"keep_doing"<<std::endl;
                                    return listener.accept().then([](seastar::connected_socket fd, seastar::socket_address addr)mutable{
                                        auto conn = seastar::make_lw_shared<connection>(std::move(fd), addr);
                                        seastar::do_until(
                                                [conn](){
                                                    return conn->_in.eof();
                                                },
                                                [conn](){
                                                    return conn->_in.read().then([conn](seastar::temporary_buffer<char>&& data)mutable{
                                                        using namespace std::chrono_literals;
                                                        return seastar::sleep(5s).then([conn,data=data.clone()]{
                                                            conn->_out.write(data.get()).then([conn]{
                                                                conn->_out.flush();
                                                            });
                                                        });

                                                    });
                                                }
                                        ).handle_exception([](std::exception_ptr ep){
                                            std::cout<<ep<<std::endl;
                                        }).finally([conn]{
                                            return conn->_out.close().finally([]{});
                                        });
                                    });
                                });
                            }
    );
};
inline
auto proc(uint16_t _port){
    return seastar::do_with(seastar::engine().listen(seastar::make_ipv4_address({_port}),{true}),
                            [](auto& listener){
                                return seastar::keep_doing([&listener]{
                                    std::cout<<"keep_doing"<<std::endl;
                                    return listener.accept().then([](seastar::connected_socket fd, seastar::socket_address addr)mutable{
                                        auto conn = seastar::make_lw_shared<connection>(std::move(fd), addr);
                                        seastar::do_until(
                                                [conn](){
                                                    return conn->_in.eof();
                                                },
                                                [conn](){
                                                    return conn->_in.read().then([conn](seastar::temporary_buffer<char>&& data)mutable{
                                                        conn->_out.write(data.get()).then([conn]{
                                                            conn->_out.flush();
                                                        });
                                                    });
                                                }
                                        ).handle_exception([](std::exception_ptr ep){
                                            std::cout<<ep<<std::endl;
                                        }).finally([conn]{
                                            return conn->_out.close().finally([]{});
                                        });
                                    });
                                });
                            }
    );
};

#endif //BOXED_HTTP_SERVICE_H
