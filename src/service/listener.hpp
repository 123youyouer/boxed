//
// Created by anyone on 18-8-18.
//

#ifndef BOXED_LISTENER_HPP
#define BOXED_LISTENER_HPP

#include "core/reactor.hh"
#include "core/future-util.hh"

template<typename CONN_PROC>
constexpr
auto listen_proc=[](uint16_t _port){
    return [&_port](CONN_PROC&& _proc){
        return seastar::do_with(seastar::engine().listen(seastar::make_ipv4_address({_port}),{true}),
                                [&_proc](auto& listener){
                                    return seastar::keep_doing([&listener,&_proc]{
                                        std::cout<<"keep_doing"<<std::endl;
                                        return listener.accept().then([&_proc](seastar::connected_socket fd, seastar::socket_address addr)mutable{
                                            _proc(fd,addr);
                                        });
                                    });
                                }
        );
    };
};

#endif //BOXED_LISTENER_HPP
