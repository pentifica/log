#include    <RingBuffer.h>

#include    <gtest/gtest.h>

#include    <thread>
#include    <vector>
#include    <any>

namespace {
    struct Element {
        int id{-9999999};
    };

    void Release(Element* e) { delete e; }

    using ElementDel = void(*)(Element*);
    using ElementRef = std::unique_ptr<Element, ElementDel>;
}

TEST(Test_RingBuffer, ctor) {
    using namespace pentifica::log;

    constexpr size_t capacity{20};

    RingBuffer<Element> queue{capacity};

    EXPECT_EQ(queue.Capacity(), capacity);

    EXPECT_EQ(queue.Length(), 0);

    EXPECT_TRUE(queue.Empty());

    EXPECT_FALSE(queue.Dequeue());
}

TEST(Test_RingBuffer, no_overrun) {
    using namespace pentifica::log;

    constexpr size_t capacity{20};
    RingBuffer<Element> queue(capacity);

    for(size_t i = 0; i < capacity; i++) {
        queue.Enqueue(Element{static_cast<int>(i)});
        EXPECT_EQ(queue.Length(), i + 1);
    }

    for(size_t i = 0; i < capacity; i++) {
        auto result = queue.Dequeue();
        EXPECT_EQ(static_cast<int>(i), result->id);
        EXPECT_EQ(queue.Length(), capacity - i - 1);
    }

    EXPECT_TRUE(queue.Empty());
}

TEST(Test_RingBuffer, overrun) {
    using namespace pentifica::log;

    constexpr size_t capacity{20};
    constexpr size_t overrun{5};

    RingBuffer<Element> queue(capacity);

    for(size_t i = 0; i < (capacity + overrun); ++i) {
        queue.Enqueue(Element{static_cast<int>(i)});
    }
    EXPECT_EQ(queue.Length(), capacity);

    for(size_t i = overrun; i < capacity; i++) {
        auto result = queue.Dequeue();
        EXPECT_EQ(result->id, static_cast<int>(i));
    }

    for(size_t i = 0; i < overrun; i++) {
        auto result = queue.Dequeue();
        EXPECT_EQ(result->id, static_cast<int>(i + capacity));
    }

    EXPECT_TRUE(queue.Empty());
}

TEST(Test_RingBuffer, threading) {
    using namespace pentifica::log;

    constexpr size_t capacity{50000};
    constexpr size_t thread_count{50};
    constexpr size_t message_count{1000};

    RingBuffer<Element> queue(capacity);

    auto task = [&] {
        for(size_t i = 0; i < capacity; i += message_count) {
            for(size_t j = 0; j < message_count; j++) {
                queue.Enqueue(Element{static_cast<int>(i + j)});
                EXPECT_FALSE(queue.Empty());
            }

            std::this_thread::sleep_for(std::chrono::microseconds(1));

            for(size_t j = 0; j < message_count; j++) {
                auto result = queue.Dequeue();
                EXPECT_GT(result->id, -1);
            }

            std::this_thread::sleep_for(std::chrono::microseconds(1));
        }
    };

    std::vector<std::thread> threads;
    threads.reserve(thread_count);
    for(size_t i = 0; i < thread_count; i++) threads.emplace_back(task);

    for(auto& thread : threads) thread.join();

    EXPECT_TRUE(queue.Empty());
}

TEST(Test_RingBuffer, unique) {
    using namespace pentifica::log;

    constexpr size_t capacity = 20;
}