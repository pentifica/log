#pragma once
/// @copyright {2023, Russell J. Fleming. All rights reserved.}
///
/// Permission is hereby granted, free of charge, to any person obtaining a copy
/// of this software and associated documentation files (the "Software"), to deal
/// in the Software without restriction, including without limitation the rights
/// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
/// copies of the Software, and to permit persons to whom the Software is
/// furnished to do so, subject to the following conditions:
///
/// The above copyright notice and this permission notice shall be included in
/// all copies or substantial portions of the Software.
///
/// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
/// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
/// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
/// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
/// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
/// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
/// SOFTWARE.

#include <Event.h>
#include <RingBuffer.h>

#include <memory>
#include <iostream>
#include <atomic>

namespace pentifica::log {
/// @brief  A multi-threaded manager for aggregating and streaming Events. The
///         design uses a circular lock-free queue to aggregate incoming events.
///         On demand, the manage will stream events to a designated stream. If
///         the queue is full, the next event results in the loss of the oldest
///         event in the queue.
class Manager {
    /// @brief  Provides a wrapper around an Event for storing incoming
    ///         events in a queue
    struct Wrapper {
        EventRef event_;
        Wrapper() : event_{nullptr, nullptr} {}
        Wrapper(Wrapper const&) : event_{nullptr, nullptr} {}
        Wrapper(Wrapper&& other) : event_(std::move(other.event_)) {}
        Wrapper(EventRef&& event) : event_(std::move(event)) {}
        ~Wrapper() = default;
        Wrapper& operator=(Wrapper const&) = delete;
        Wrapper& operator=(Wrapper&& other) {
            event_ = std::move(other.event_);
            return *this;
        }
    };
    using EventRingBuffer = RingBuffer<Wrapper>;

public:
    /// @brief  Prepare an event manager that can enqueue, at most, capacity
    ///         events without overrun.
    /// @param os           Where to stream events
    /// @param capacity     The max number of events that can be enqueued
    ///                     before older events are overwritten
    explicit Manager(std::ostream& os, size_t capacity);
    /// @brief  Deleted
    Manager(Manager const&) = delete;
    /// @brief  Deleted
    Manager(Manager&&) = delete;
    /// @brief  Default
    ~Manager() = default;
    /// @brief  Enqueue a log event.
    /// @param  event   Enqueue the log event.
    void Enqueue(EventRef&& event) {
        queue_->Enqueue(Wrapper(std::move(event)));
        ++events_received_;
    }
    /// @brief  Stream, at most, the configured number of Event messages from
    ///         the internal queue.
    /// @param  count   Max number of messages to stream from the queue
    void Flush(size_t count);
    /// @brief  Stream all Event messages from the inernal queue.
    void Dump();
    /// @brief  Clear all Events from the internal queue.
    void Clear() { queue_->Clear(); }
    auto Received() const {
        return events_received_.load(std::memory_order_relaxed);
    }
    auto Published() const {
        return events_published_.load(std::memory_order_relaxed);
    }
    /// @brief  Deleted
    Manager& operator=(Manager const&) = delete;
    /// @brief  Deleted
    Manager& operator=(Manager&&) = delete;

private:
    /// @brief  Where to stream events
    std::ostream& os_;
    /// @brief  Where events are queued prior to streaming
    std::unique_ptr<EventRingBuffer> queue_;
    /// @brief  Total number of events received
    std::atomic<size_t> events_received_{};
    /// @brief  Total number of events streamed
    std::atomic<size_t> events_published_{};
};
}