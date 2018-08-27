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
        std::unique_ptr<const char> content;
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
        char* new_response_line(){
            return new char[1024];
        }

    };
    /*
    constexpr
    auto build_response_line=[](const status_type& status){
        return [s=status](std::unique_ptr<char>& ver){
            return [&s,v=std::move(ver)](std::unique_ptr<char>& content_type){
                return [&s,&v,t=std::move(content_type)](std::unique_ptr<char>& content){
                    return [&s,&v,&t,c=std::move(content)](char* buf){
                        char* w_ptr=buf;
                        sprintf(w_ptr,"%s\r\n%s\r\n%s\r\n%s",
                                response_builder::get_status_string(s),
                                v,
                                t,
                                c);
                        return buf;
                    };
                };
            };

        };
    };
*/

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
        void init_cb(){
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
        char* base_line;
        size_t base_line_len;
        size_t base_line_used;
        void do_parser(){
            http_parser_init(&parser,HTTP_REQUEST);
            parser.data=this;
            http_parser_execute(&parser,&settings,base_line,base_line_used);
        }
    public:
        http_parser_settings settings;
        http_parser parser;
        std::string method;
        std::string url;
        std::string body;
        //std::vector<http_header&> headers;
        unsigned int flags;
        unsigned short http_major, http_minor;
        const http_request& reset(const char* request_line,size_t len){
            if(base_line_len<(len+1)){
                delete base_line;
                base_line_len=len+1;
                base_line=new char[base_line_len];
            }
            memcpy(base_line,request_line,len);
            base_line_used=len;
            do_parser();
            return *this;
        }
        http_request():base_line(new char[base_line_len]),base_line_len(10240),base_line_used(0){
            init_cb();
        }
    };

    struct http_service_connection_context : public connection{
    public:
        http_request req;
        http_response res;
        http_service_connection_context(seastar::connected_socket&& socket, seastar::socket_address addr)
                :connection(std::move(socket),addr){
            std::cout<<"build s1"<<std::endl;
        }
        virtual ~http_service_connection_context(){
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
    auto connection_proc(REQ_HANDLER&& _handler,seastar::connected_socket& fd, seastar::socket_address& addr){
        return seastar::do_with(
                UNIQUE_PTR_WRAPPER(new http_service_connection_context(std::move(fd),addr)),
                [&_handler](UNIQUE_PTR_WRAPPER<http_service_connection_context>& ctx){
                    return seastar::do_until(
                            [&ctx]()->bool{
                                return ctx.p->_in.eof();
                            },
                            [&ctx,&_handler](){
                                return ctx.p->_in.read()
                                        .then([&ctx,&_handler](seastar::temporary_buffer<char>&& data){
                                            if(data.empty())
                                                return seastar::make_ready_future();
                                            else
                                                return seastar::make_ready_future(ctx.p->res.body=_handler(ctx.p->req.reset(data.begin(),data.size())))
                                                .then([&ctx](std::string&& r){
                                                    ctx.p->_out.write(r)
                                                            .then([&ctx](){
                                                                return ctx.p->_out.flush().then([](){});
                                                            }).then(([&ctx](){
                                                                return ctx.p->_out.close();
                                                            }));
                                                })
                                        });
                            }
                    );
                }
        ).finally([](){std::cout<<"connection closed"<<std::endl;});
    };


}

#endif //BOXED_HTTP_SERVICE_H
