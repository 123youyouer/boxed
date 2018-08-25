//
// Created by null on 18-8-15.
//

#ifndef BOXED_HTTP_SERVICE_H
#define BOXED_HTTP_SERVICE_H

#include <boost/variant.hpp>
#include "connection.hpp"
#include "http-parser/http_parser.h"

namespace http_service{
    struct http_exp{
        std::string a;
        http_exp(const char* sz):a(sz){
            std::cout<<"build q2"<<std::endl;
        }
        http_exp(){
            std::cout<<"build q1"<<std::endl;
        }
        ~http_exp(){
            std::cout<<"delete q1"<<std::endl;
        }



    };
    struct http_header {
        char* name;
        char* value;
    };
    struct http_response{
        std::string body;
    };
    struct http_request{
        std::string method;
        std::string url;
        std::string body;
        unsigned int flags;
        unsigned short http_major, http_minor;
        struct http_header *headers;
    };

    struct http_connection_context : public connection{
    private:
        static int cb_on_message_begin(http_parser* p){
            return 0;
        }
        static int cb_on_url(http_parser* p, const char *at, size_t length){
            (static_cast<http_request*>(p->data)->url=at).resize(length);
            return 0;
        }
        static int cb_on_status(http_parser* p, const char *at, size_t length){
            return 0;
        }
        static int cb_on_header_field(http_parser* p, const char *at, size_t length){
            return 0;
        }
        static int cb_on_header_value(http_parser* p, const char *at, size_t length){
            return 0;
        }
        static int cb_on_headers_complete(http_parser* p){
            return 0;
        }
        static int cb_on_body(http_parser* p, const char *at, size_t length){
            return 0;
        }
        static int cb_on_message_complete(http_parser* p){
            return 0;
        }
        static int cb_on_chunk_header(http_parser* p){
            return 0;
        }
        static int cb_on_chunk_complete(http_parser* p){
            return 0;
        }
    public:
        http_request current_req;
        http_response current_res;
        http_parser req_parser;
        http_parser_settings settings;
        http_connection_context(seastar::connected_socket&& socket, seastar::socket_address addr)
                :connection(std::move(socket),addr){
            std::cout<<"build s1"<<std::endl;
            settings.on_message_begin=cb_on_message_begin;
            settings.on_url=cb_on_url;
            settings.on_status=cb_on_status;
            settings.on_header_field=cb_on_header_field;
            settings.on_header_value=cb_on_header_value;
            settings.on_headers_complete=cb_on_headers_complete;
            settings.on_body=cb_on_body;
            settings.on_message_complete=cb_on_message_complete;
            settings.on_chunk_header=cb_on_chunk_header;
            settings.on_chunk_complete=cb_on_chunk_complete;
        }
        virtual ~http_connection_context(){
            std::cout<<"delete s2"<<std::endl;
        }
    };

    template <typename ...T>
    struct AAA{

    };

    template <typename INNER>
    struct UNIQUE_PTR_WRAPPER{
        std::unique_ptr<INNER> p;
        UNIQUE_PTR_WRAPPER(INNER* _p):p(_p){};
        UNIQUE_PTR_WRAPPER(UNIQUE_PTR_WRAPPER<INNER>&& o){p=std::move(o.p);}
    };

    constexpr
    auto build_requset=[](http_parser&& parser
            , http_parser_settings&& settings
            , seastar::temporary_buffer<char>&& data
            , http_request&& req){
        http_parser_init(&parser,HTTP_REQUEST);
        parser.data=&req;
        http_parser_execute(&parser,&settings,data.begin(),data.size());
        return seastar::make_ready_future<http_request&&>(std::forward<http_request&&>(req));
    };

    constexpr
    auto build_response=[](http_response&& res,const char* s){
        res.body=s;
        return seastar::make_ready_future<http_response&&>(std::forward<http_response&&>(res));
    };


    template <typename REQ_HANDLER>
    constexpr
    auto build_connection_proc(REQ_HANDLER&& _handler)noexcept{
        return [&_handler](seastar::connected_socket& fd, seastar::socket_address& addr){
            return seastar::do_with(UNIQUE_PTR_WRAPPER(new http_connection_context(std::move(fd),addr)),
                                    [&_handler](auto& conn){
                                        return seastar::do_until(
                                                [&conn]{
                                                    return conn.p->_in.eof();
                                                },
                                                [&conn,_handler=std::forward<REQ_HANDLER>(_handler)]{
                                                    return conn.p->_in.read().then([&conn,&_handler](seastar::temporary_buffer<char>&& data)mutable{
                                                        if(!data.empty()){
                                                            return build_requset(
                                                                    std::move(conn.p->req_parser),
                                                                    std::forward<http_parser_settings>(conn.p->settings),
                                                                    std::forward<seastar::temporary_buffer<char>>(data),
                                                                    std::forward<http_request>(conn.p->current_req))
                                                                    .then([&conn,&_handler](http_request&& req){
                                                                        return build_response(
                                                                                std::forward<http_response>(conn.p->current_res),
                                                                                _handler(std::move(req)));
                                                                    })
                                                                    .then([&conn](http_response&& res){
                                                                        return conn.p->_out.write(res.body);
                                                                    });
                                                        }else{
                                                            return seastar::make_ready_future();
                                                        }
                                                    }).then([&conn](){
                                                        return conn.p->_out.flush();
                                                        //return conn._out.close().finally([]{});
                                                    });
                                                }
                                        ).handle_exception([](std::exception_ptr ep){
                                        }).finally([&conn]{
                                            return conn.p->_out.close().finally([]{});
                                        });

                                    }
            );

        };
    };
}

#endif //BOXED_HTTP_SERVICE_H
