#include "test_hana.hpp"

int main() {
    boost::any a = 10l;
    std::string r = switch_(a)(
            case_<int>([](auto i) {
                return "1";
            }),
            case_<char>([](auto c) {
                return "2";
            }),
            case_<long>([](auto l) {
                return "3";
            }),
            case_<std::string>([](auto l) { return "4"; }),
            case_<std::string*>([](auto l) { return "5"; }),
            default_([] { return "6"; })
    );
    std::cout << r << std::endl;
    return 0;
}