//
// Created by null on 18-8-15.
//

#ifndef BOXED_HTTP_SERVICE_H
#define BOXED_HTTP_SERVICE_H

#include <boost/variant.hpp>
#include "connection.hpp"
#include "http-parser/http_parser.h"
#include "../utils/unique_ptr_wrapper.hpp"

namespace http_service{
    enum class status_type {
        ok = 200, //!< ok
        created = 201, //!< created
        accepted = 202, //!< accepted
        no_content = 204, //!< no_content
        multiple_choices = 300, //!< multiple_choices
        moved_permanently = 301, //!< moved_permanently
        moved_temporarily = 302, //!< moved_temporarily
        not_modified = 304, //!< not_modified
        bad_request = 400, //!< bad_request
        unauthorized = 401, //!< unauthorized
        forbidden = 403, //!< forbidden
        not_found = 404, //!< not_found
        internal_server_error = 500, //!< internal_server_error
        not_implemented = 501, //!< not_implemented
        bad_gateway = 502, //!< bad_gateway
        service_unavailable = 503  //!< service_unavailable
    };

    template <status_type T>
    constexpr
    const char* buildres=NULL;

    template <> constexpr const char* buildres<status_type::ok> = " 200 OK\r\n";
    template <> constexpr const char* buildres<status_type::created> = " 201 Created\r\n";
    template <> constexpr const char* buildres<status_type::accepted> = " 202 Accepted\r\n";
    template <> constexpr const char* buildres<status_type::no_content> = " 204 No Content\r\n";
    template <> constexpr const char* buildres<status_type::multiple_choices> = " 300 Multiple Choices\r\n";
    template <> constexpr const char* buildres<status_type::moved_permanently> = " 301 Moved Permanently\r\n";
    template <> constexpr const char* buildres<status_type::moved_temporarily> = " 302 Moved Temporarily\r\n";
    template <> constexpr const char* buildres<status_type::not_modified> = " 304 Not Modified\r\n";
    template <> constexpr const char* buildres<status_type::bad_request> = " 400 Bad Request\r\n";
    template <> constexpr const char* buildres<status_type::unauthorized> = " 401 Unauthorized\r\n";
    template <> constexpr const char* buildres<status_type::forbidden> = " 403 Forbidden\r\n";
    template <> constexpr const char* buildres<status_type::not_found> = " 404 Not Found\r\n";
    template <> constexpr const char* buildres<status_type::internal_server_error> = " 500 Internal Server Error\r\n";
    template <> constexpr const char* buildres<status_type::not_implemented> = " 501 Not Implemented\r\n";
    template <> constexpr const char* buildres<status_type::bad_gateway> = " 502 Bad Gateway\r\n";
    template <> constexpr const char* buildres<status_type::service_unavailable> = " 503 Service Unavailable\r\n";

    struct response_builder{
    public:
        const static char* get_status_string(const status_type& status) {
            switch (status) {
                case status_type::ok:
                    return buildres<status_type::ok>;
                case status_type::created:
                    return buildres<status_type::created>;
                case status_type::accepted:
                    return buildres<status_type::accepted>;
                case status_type::no_content:
                    return buildres<status_type::no_content>;
                case status_type::multiple_choices:
                    return buildres<status_type::multiple_choices>;
                case status_type::moved_permanently:
                    return buildres<status_type::moved_permanently>;
                case status_type::moved_temporarily:
                    return buildres<status_type::moved_temporarily>;
                case status_type::not_modified:
                    return buildres<status_type::not_modified>;
                case status_type::bad_request:
                    return buildres<status_type::bad_request>;
                case status_type::unauthorized:
                    return buildres<status_type::unauthorized>;
                case status_type::forbidden:
                    return buildres<status_type::forbidden>;
                case status_type::not_found:
                    return buildres<status_type::not_found>;
                case status_type::internal_server_error:
                    return buildres<status_type::internal_server_error>;
                case status_type::not_implemented:
                    return buildres<status_type::not_implemented>;
                case status_type::bad_gateway:
                    return buildres<status_type::bad_gateway>;
                case status_type::service_unavailable:
                    return buildres<status_type::service_unavailable>;
                default:
                    return buildres<status_type::internal_server_error>;
            }
        }
        std::string _status;
        std::string _body;
    public:
        response_builder(char* buf){}
        response_builder& set_status(const status_type& status){
            _status=get_status_string(status);
            return *this;
        }
        response_builder& set_content(const char* body){
            return *this;
        }
        response_builder& set_content_type(const char* type){
            return *this;
        }
        response_builder& set_version(const char* ver){
            return *this;
        }
        char* line(){
            return new char[1024];
        }

    };
    constexpr
    auto build_response_line=[](const status_type& status){
        return [&status](const char* ver){
            return [&status,&ver](const char* content_type){
                return [&status,&ver,&content_type](const char* content){
                    return [&status,&ver,&content_type,content](char* buf){
                        char* w_ptr=buf;
                        w_ptr=strcpy(w_ptr,response_builder::get_status_string(status));
                        w_ptr=strcpy(w_ptr,ver);
                        w_ptr=strcpy(w_ptr,"\r\n");
                        w_ptr=strcpy(w_ptr,content_type);
                        w_ptr=strcpy(w_ptr,"\r\n");
                        w_ptr=strcpy(w_ptr,content);
                        return buf;
                    };
                };
            };

        };
    };


    struct http_response{

        status_type status;
        std::string body;
        http_response():status(status_type::ok){

        }
    };


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
        std::string name;
        std::string value;
    };
    struct http_request{
        std::string method;
        std::string url;
        std::string body;
        //std::vector<http_header&> headers;
        unsigned int flags;
        unsigned short http_major, http_minor;
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
                                                                        /*
                                                                        using t=decltype(build_response_line(status_type::ok)("1.1")("html")("html"));
                                                                        return seastar::make_ready_future<t&&>(std::forward<t&&>(build_response_line
                                                                                                                                         (status_type::ok)
                                                                                                                                         ("1.1")
                                                                                                                                         ("html")
                                                                                                                                         (_handler(std::move(req)))));
                                                                        */
                                                                        auto res=build_response_line
                                                                                (status_type::ok)
                                                                                ("1.1")
                                                                                ("html")
                                                                                (_handler(std::move(req)));
                                                                        char sz[1024]={0};
                                                                        return conn.p->_out.write(res(sz));

                                                                    })
                                                                    .then([&conn](){
                                                                        return conn.p->_out.flush();
                                                                        //char sz[1024]={0};
                                                                        //return conn.p->_out.write(res(sz));
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
