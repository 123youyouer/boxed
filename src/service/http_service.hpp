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
        int a=0;
        http_exp(){
            std::cout<<"build q1"<<std::endl;
        }


    };
    struct http_header {
        char* name;
        char* value;
    };
    struct http_response{

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
        http_request last_request;
        http_parser req_parser;
        http_parser_settings settings;
        http_connection_context(seastar::connected_socket&& socket, seastar::socket_address addr)
                :connection(std::move(socket),addr){
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
    auto build_response=[](http_response&& res){
        return seastar::make_ready_future<http_response&&>(std::forward<http_response&&>(res));
    };

    constexpr
    auto build_parser=[](http_connection_context&& ctx, seastar::temporary_buffer<char>&& data){
        return [&ctx,&data](http_request&& req){
            http_parser_init(&ctx.req_parser,HTTP_REQUEST);
            http_parser_execute(&ctx.req_parser,&ctx.settings,data.begin(),data.size());
            return seastar::make_ready_future<http_request>(req);
        };
    };
    constexpr
    auto connection_proc=[](seastar::connected_socket& fd, seastar::socket_address& addr){
        return seastar::do_with(http_connection_context(std::move(fd),addr),
                                [](http_connection_context& conn){
                                    return seastar::do_until(
                                            [&conn]{
                                                return conn._in.eof();
                                            },
                                            [&conn]{
                                                return conn._in.read().then([&conn](seastar::temporary_buffer<char>&& data)mutable{
                                                    if(!data.empty()){
                                                        return build_requset(std::move(conn.req_parser)
                                                                ,std::move(conn.settings)
                                                                ,std::move(data)
                                                                ,std::forward<http_request>(conn.last_request))
                                                                .then([](http_request&& req){
                                                                    return seastar::make_ready_future<std::string&&>("11111");
                                                                })
                                                                .then([&conn](std::string&& body){
                                                                    conn._out.write(body);
                                                                });
                                                    }
                                                    return seastar::make_ready_future();
                                                }).then([&conn](){
                                                    conn._out.flush();
                                                    return conn._out.close().finally([]{});
                                                });
                                            }
                                    ).handle_exception([](std::exception_ptr ep){
                                    }).finally([]{
                                        //return conn._out.close().finally([]{});
                                    });
                                }
        );
    };
    using http_connection_proc_type=decltype(http_service::connection_proc);
}

#endif //BOXED_HTTP_SERVICE_H
