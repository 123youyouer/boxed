//
// Created by null on 18-8-30.
//

#ifndef BOXED_API_CONSOLE_HPP
#define BOXED_API_CONSOLE_HPP
#include "api_aware_base.hpp"
#include "../utils/null_terminated_ptr.hpp"
namespace vm{
    class api_console : public api_aware_base{
        void prints(null_terminated_ptr ptr){
        }
    };
}
#endif //BOXED_API_CONSOLE_HPP
