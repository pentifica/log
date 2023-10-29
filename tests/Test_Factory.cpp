#include    <Factory.h>
#include    <GenericEvent.h>

#include    <gtest/gtest.h>

#include    <thread>
#include    <chrono>
#include    <vector>
#include    <iostream>
#include    <random>

namespace {
    static size_t a_count{};
    static size_t b_count{};
    struct BasicA final : public pentifica::log::Event {
        using Event::Event;
        BasicA(int a, float b, std::string c) :
            Event(), a_{a}, b_{b}, name_{std::move(c)} { ++a_count; }
        BasicA(std::string s) : Event(), name_{std::move(s)} { ++a_count; }
        virtual ~BasicA() { --a_count; }

        void Log(std::ostream&) const override {}

    private:
        int a_{};
        float b_{};
        std::string name_{};
    };

    struct BasicB final : public pentifica::log::Event {
        using Event::Event;
        BasicB(const char* a) : Event(), name_{a} { ++b_count; }
        BasicB(std::string a) : Event(), name_{std::move(a)} { ++b_count; }
        virtual ~BasicB() { --b_count; }

        void Log(std::ostream&) const override {}

    private:
        std::string name_{};
    };

    struct MultiUserTest final : public pentifica::log::Event {
        using Event::Event;
        MultiUserTest(const char* a) : name_{a} {}
        MultiUserTest(std::string a) : name_{std::move(a)} {}

        void Log(std::ostream&) const override {}

        std::string const& Name() const { return name_; }

        MultiUserTest(MultiUserTest const&) = default;
        MultiUserTest(MultiUserTest&&) = default;
        MultiUserTest& operator=(MultiUserTest const&) = default;
        MultiUserTest& operator=(MultiUserTest&&) = default;

    private:
        std::string name_{};
    };

    using namespace pentifica::log;
    using FactoryA = Factory<BasicA>;
    using FactoryB = Factory<BasicB>;
}

TEST(Test_Factory, default) {
    {
        auto a = FactoryA::Create(1, 2.0, std::string{"BasicA"});
        EXPECT_EQ(FactoryA::Capacity(), 1);
        EXPECT_EQ(FactoryA::Available(), 0);
        EXPECT_EQ(a_count, 1);
    }
    EXPECT_EQ(a_count, 0);
    
    {
        auto b = FactoryB::Create("BasicB");
        EXPECT_EQ(FactoryB::Capacity(), 1);
        EXPECT_EQ(FactoryB::Available(), 0);
    }
}

TEST(Test_Factory, CheckCapacity) {
    auto initial_capacity {FactoryA::Capacity()};
    auto initial_available {FactoryA::Available()};

    constexpr size_t capacity {20};
    FactoryA::AddCapacity(capacity);
    EXPECT_EQ(FactoryA::Capacity(), capacity + initial_capacity);
    EXPECT_EQ(FactoryA::Available(), capacity + initial_available);
}

TEST(Test_Factory, Usage) {
    auto initial_capacity {FactoryA::Capacity()};
    auto initial_available {FactoryA::Available()};

    constexpr size_t additional_capacity{20};
    FactoryA::AddCapacity(additional_capacity);
    EXPECT_EQ(FactoryA::Capacity(), additional_capacity + initial_capacity);
    EXPECT_EQ(FactoryA::Available(), additional_capacity + initial_available);

    using Cache = std::vector<EventRef>;
    Cache cache;
    auto create_count = FactoryA::Available();
    for(size_t i = 0; i < create_count; i++) {
        cache.emplace_back(FactoryA::Create("test"));
    }
    EXPECT_EQ(FactoryA::Capacity(), cache.size());
    EXPECT_EQ(FactoryA::Available(), 0);

    cache.clear();
    EXPECT_EQ(FactoryA::Capacity(), FactoryA::Available());
}


TEST(Test_Factory, multiuser) {
    using namespace pentifica::log;
    using namespace std::chrono_literals;

    constexpr size_t initial_capacity{20};
    constexpr size_t users{80};
    constexpr size_t cycles{1000};

    using TestFactory = Factory<MultiUserTest>;

    TestFactory::AddCapacity(initial_capacity);
    EXPECT_EQ(TestFactory::Capacity(), initial_capacity);
    EXPECT_EQ(TestFactory::Available(), initial_capacity);

    std::random_device dev;
    std::uniform_int_distribution<std::mt19937::result_type> dist(10, 75);

    auto user = [&] () {
        thread_local std::mt19937 rng(dev());
        std::ostringstream oss;
        oss << "Thread " << std::this_thread::get_id();
        std::this_thread::sleep_for(std::chrono::milliseconds(dist(rng)));

        for(size_t cycle = 0; cycle < cycles; ++cycle) {
            auto event{TestFactory::Create(oss.str())};
            std::this_thread::sleep_for(std::chrono::milliseconds(dist(rng)));
            EXPECT_TRUE(static_cast<MultiUserTest*>(event.get())->Name() == oss.str());
        }
    };

    std::vector<std::thread> threads(users);
    for(auto& thread : threads) thread = std::thread(user);
    for(auto& thread : threads) thread.join();

    EXPECT_EQ(TestFactory::Capacity(), TestFactory::Available());
    EXPECT_LE(TestFactory::Capacity(), users);
}

TEST(Test_Factory, generic) {
    using namespace pentifica::log;
    constexpr size_t initial_capacity{20};

    using TestGenericEvent = GenericEvent<std::string, int, std::string, double>;

    using GenericFactory = Factory<TestGenericEvent>;

    GenericFactory::AddCapacity(initial_capacity);
    EXPECT_EQ(initial_capacity, GenericFactory::Capacity());

    auto event = GenericFactory::Create("this is an int: ", 21, ", this is a double: ", 7.8);
    // auto bad_event = GenericFactory::Create(5, 19, 3.4, 5.9);

    std::ostringstream oss;
    oss << *event;
    EXPECT_TRUE(oss.str().find("this is an int: 21, this is a double: 7.8") != std::string::npos);
}