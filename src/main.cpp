//
// Created by null on 18-8-15.
//
#include "core/app-template.hh"
#include "core/reactor.hh"
#include "core/sleep.hh"
#include "core/thread.hh"
#include <iostream>
using namespace std::chrono_literals;
int main(int argc, char** argv) {
    seastar::app_template app;
    app.run(argc, argv, []{
        return seastar::sleep(1s).then([]{
            std::cout<<"hello world"<<std::endl;
        });
    });
}