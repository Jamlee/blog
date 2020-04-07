#include <iostream>

#define CATCH_CONFIG_MAIN 
#include "../deps/catch.hpp"

// auto 作为返回值时， 强制指定返回类型
auto sayHello1() -> int {
    return 1;
}

// auto 作为返回值时， 自动推理指定返回类型
auto sayHello2() {
    return 1;
}

TEST_CASE( "指定函数返回值为 auto", "[c++11]" ) {
    REQUIRE( sayHello1() == 1 );
    REQUIRE( sayHello2() == 1 );
}
