//
// Created by null on 18-8-15.
//
#include "core/app-template.hh"
#include "core/reactor.hh"
#include "core/sleep.hh"
#include "core/thread.hh"
#include <iostream>
#include "service/http_service.hpp"
#include "service/listener.hpp"


using namespace std::chrono_literals;
int main(int argc, char** argv) {
    seastar::app_template app;
    app.run(argc, argv, []{
        []{
            seastar::when_all_succeed(
                    listen_proc<http_service::http_connection_proc_type>(8080)(std::move(http_service::connection_proc))
            ).then([]{
                std::cout<<"1111"<<std::endl;
            });
        }();
        return seastar::sleep(1000s).then([]{});


    });
}