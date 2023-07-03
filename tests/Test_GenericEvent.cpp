#include <GenericEvent.h>

#include <gtest/gtest.h>

#include <sstream>

TEST(Test_GenericEvent, basic) {
    using namespace pentifica::log;

    GenericEvent event{"result=", 34.9, ", number of elements=", 5};
    constexpr char const* const expected = "result=34.9, number of elements=5";
    std::ostringstream oss;
    oss << event;
    EXPECT_NE(oss.str().find(expected), std::string::npos);
}