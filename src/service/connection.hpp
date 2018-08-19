//
// Created by anyone on 18-8-18.
//

#ifndef BOXED_CONNECTION_HPP
#define BOXED_CONNECTION_HPP

#include "core/iostream.hh"
#include "net/api.hh"

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
    connection(connection&& o)
            : _socket(std::move(o._socket))
            , _addr(o._addr)
            , _in(_socket.input())
            , _out(_socket.output())
    {}
    ~connection() {}
};

#endif //BOXED_CONNECTION_HPP
