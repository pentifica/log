#include <Manager.cpp>
#include <Factory.h>

#include <gtest/gtest.h>

#include <string>
#include <iostream>

namespace {
    struct Capture final : public pentifica::log::Event {
        std::string text_{};
        using Event::Event;
        template<typename T>
        Capture(T&& text) {
            if constexpr (std::is_pointer<char*>::value)
                text_ = text;
            else
                text_ = std::move(text);
        }
        Capture(Capture const& other) : text_{other.text_} {}
        Capture(Capture&& other) : text_{std::move(other.text_)} {}
        ~Capture() override = default;
        Capture& operator=(Capture const&) = default;
        Capture& operator=(Capture&&) = default;

        void Log(std::ostream& os) const override { os << text_; }
        void Reset() { text_.clear(); }
    };

    using CaptureFactory = pentifica::log::Factory<Capture>;
    
    constexpr size_t capacity = 20;

    std::vector<std::string> const messages = {
        "line 1",
        "line 2",
        "line 3",
        "line 4"
    };

}

TEST(Test_Manager, ctor) {
    using namespace pentifica::log;

    std::ostringstream oss;

    Manager manager(oss, capacity);

    EXPECT_EQ(manager.Received(), 0);
    EXPECT_EQ(manager.Published(), 0);
}

TEST(Test_Manager, received) {
    using namespace pentifica::log;

    std::ostringstream oss;

    Manager manager(oss, capacity);

    for(auto& message : messages) {
        manager.Enqueue(CaptureFactory::Create(message));
    }
    EXPECT_EQ(manager.Received(), messages.size());
    EXPECT_EQ(manager.Published(), 0);
    EXPECT_TRUE(oss.str().empty());

    manager.Clear();
}

TEST(Test_Manager, flush) {
    using namespace pentifica::log;

    std::ostringstream oss;

    Manager manager(oss, capacity);

    for(auto const& message : messages) {
        manager.Enqueue(CaptureFactory::Create(message));
    }

    manager.Flush(1);
    EXPECT_EQ(manager.Published(), 1);
    EXPECT_TRUE(oss.str().find(messages[0]) != std::string::npos);
    EXPECT_TRUE(oss.str().find(messages[1]) == std::string::npos);

    manager.Flush(messages.size() + capacity);
    EXPECT_EQ(manager.Published(), messages.size());
    for(auto const& message : messages) {
        EXPECT_TRUE(oss.str().find(message) != std::string::npos);
    }

    manager.Clear();
}

TEST(Test_Manager, dump) {
    using namespace pentifica::log;

    std::ostringstream oss;

    Manager manager(oss, capacity);

    for(auto const& message : messages) {
        manager.Enqueue(CaptureFactory::Create(message));
    }

    manager.Flush(1);
    manager.Dump();
    EXPECT_EQ(manager.Published(), messages.size());
    for(auto const& message : messages) {
        EXPECT_TRUE(oss.str().find(message) != std::string::npos);
    }

    manager.Clear();
}

TEST(Test_Manager, clear) {
    using namespace pentifica::log;

    std::ostringstream oss;

    Manager manager(oss, capacity);

    for(auto const& message : messages) {
        manager.Enqueue(CaptureFactory::Create(message));
    }

    manager.Clear();
    manager.Flush(1);
    EXPECT_EQ(manager.Published(), 0);
}