#include    <Event.h>

#include    <gtest/gtest.h>

#include    <array>
#include    <sstream>
#include    <ctime>

namespace {
    struct Basic final : public pentifica::log::Event {
        using Event::Event;
        void Log(std::ostream&) const override {}
    };
}

TEST(Test_Event, test_severity) {
    using namespace pentifica::log;
    constexpr std::array severity_list {
        Severity::Debug,
        Severity::Info,
        Severity::Tracking,
        Severity::Logic,
        Severity::Critical,
        Severity::Alert,
        Severity::Fatal
    };

    for(auto severity : severity_list) {
        Basic basic {severity};
        std::ostringstream oss;
        oss << basic;
        EXPECT_NE(oss.str().find(ToString(severity)), std::string::npos);
    }
}

TEST(Test_Event, test_time) {
    using namespace pentifica::log;

    static char const* const expected = "2023-01-14 13:47:29.004389";

    std::tm tm{};
    tm.tm_year = 2023 - 1900;
    tm.tm_mon = 0;
    tm.tm_mday = 14;
    tm.tm_hour = 13;
    tm.tm_min = 47;
    tm.tm_sec = 29;

    auto now = mktime(&tm);

    auto clock_time = Event::Clock::from_time_t(now);
    clock_time += std::chrono::microseconds(4389);

    Basic basic;
    {
        std::ostringstream oss;
        oss << basic;
        EXPECT_EQ(oss.str().find(expected), std::string::npos);
    }

    basic.Reset(clock_time);
    {
        std::ostringstream oss;
        oss << basic;
        EXPECT_NE(oss.str().find(expected), std::string::npos);
    }
}