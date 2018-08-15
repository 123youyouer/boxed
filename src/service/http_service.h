//
// Created by null on 18-8-15.
//

#ifndef BOXED_HTTP_SERVICE_H
#define BOXED_HTTP_SERVICE_H
#include "core/distributed.hh"
#include <boost/hana.hpp>

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

template<typename F>
auto listen_proc=[](uint16_t _port){
    return seastar::do_with(seastar::engine().listen(seastar::make_ipv4_address({_port}),{true}),
                     [](auto& listener){
                         return seastar::keep_doing([&listener]{
                             return listener.accept().then([](seastar::connected_socket fd, seastar::socket_address addr)mutable{
                                 return seastar::smp::submit_to(seastar::engine().cpu_id(),[fd=std::move(fd)]{
                                     return F()(fd);
                                 });
                             });
                         });
                     }
    );
};
#endif //BOXED_HTTP_SERVICE_H
