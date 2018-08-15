//
// Created by null on 18-8-15.
//
#include "core/app-template.hh"
#include "core/reactor.hh"
#include "core/sleep.hh"
#include "core/thread.hh"
#include <iostream>
#include "service/http_service.h"


using namespace std::chrono_literals;
int main(int argc, char** argv) {
    seastar::app_template app;
    app.run(argc, argv, []{
        auto s=[](const seastar::connected_socket& fd){
        std::cout<<"111111"<<std::endl;
    };
        return listen_proc<s>(8080);
    });
}