add_executable(boxed
        main.cpp
        service/http_service.h
        service/http_service.cpp)

target_compile_options(boxed PUBLIC
        -std=gnu++17 -g
        -Wall -Werror -Wno-error=deprecated-declarations
        -fvisibility=hidden -Wno-attributes -pthread
        -I/home/null/CLionProjects/boxed/seastar -U_FORTIFY_SOURCE
        -fconcepts
        -I/home/null/CLionProjects/boxed/seastar/fmt
        -DBOOST_TEST_DYN_LINK -Wno-ignored-attributes
        -Wno-overloaded-virtual -Wno-maybe-uninitialized -Wno-error=cpp -Wno-stringop-overflow
        #        -DSEASTAR_HAVE_GCC6_CONCEPTS
        -DSEASTAR_HAVE_HWLOC
        -DSEASTAR_HAVE_NUMA
        -DSEASTAR_HAS_MEMBARRIER -DSEASTAR_HAVE_LZ4_COMPRESS_DEFAULT -DSEASTAR_HAVE_ASAN_FIBER_SUPPORT
        #        -O2
        -I/home/null/seastar/seastar/build/release/gen
        -I/home/null/seastar/seastar/build/release/c-ares
        )
target_link_libraries(boxed PUBLIC
        -L/home/null/seastar/seastar/build/release
        -L/home/null/seastar/seastar/build/release/fmt/fmt
        -Wl,--whole-archive,-lseastar,--no-whole-archive
        -lfmt -std=gnu++17 -g
        -Wall -Werror -Wno-error=deprecated-declarations -fvisibility=hidden -Wno-attributes
        -pthread
        -I/home/null/CLionProjects/boxed/seastar
        -U_FORTIFY_SOURCE
        -fconcepts
        -I/home/null/CLionProjects/boxed/seastar/fmt
        -DBOOST_TEST_DYN_LINK -Wno-ignored-attributes -Wno-overloaded-virtual
        -Wno-maybe-uninitialized -Wno-error=cpp -Wno-stringop-overflow
        #        -DSEASTAR_HAVE_GCC6_CONCEPTS
        -DSEASTAR_HAVE_HWLOC
        -DSEASTAR_HAVE_NUMA
        -DSEASTAR_HAS_MEMBARRIER -DSEASTAR_HAVE_LZ4_COMPRESS_DEFAULT
        -DSEASTAR_HAVE_ASAN_FIBER_SUPPORT
        -Wl,--no-as-needed
        -lboost_program_options
        -lboost_system -lboost_filesystem -lstdc++ -lm -lstdc++fs
        -lboost_thread -lcryptopp -lrt -lgnutls -lgnutlsxx -llz4 -lprotobuf -ldl -lgcc_s
        -lyaml-cpp -lhwloc -lnuma -lpciaccess -lxml2 -lz -lcares-seastar -lfmt
        )