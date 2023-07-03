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
#include    <memory>
#include    <vector>
#include    <mutex>
#include    <iostream>
#include    <optional>

namespace pentifica::log {
/// @brief Provides a circular enque/deque mechanism for log events. If events
///        are enqued faster than dequed, older enqued events are dropped.
/// @tparam Element     The type of element to be stored on the buffer. It must
///                     support an empty ctor and be std::move'able
/// @tparam Lockable    Must conform to the BasicLockableType
///                     (default = std::mutex)
template<typename Element, typename Lockable = std::mutex>
class RingBuffer {
    using Cache = std::vector<Element>;

public:
    /// @brief Initialize
    /// @param capacity The capacity of the buffer. 
    explicit RingBuffer(size_t capacity) : cache_(capacity) {}
    /// @brief Deleted
    RingBuffer(RingBuffer const&) = delete;
    /// @brief  Move the indicated buffer into the new buffer
    /// @param buffer 
    RingBuffer(RingBuffer&& buffer) {
        next_read_ = buffer.next_read_;
        next_write_ = buffer.next_write_;
        cache_ = buffer.cache_;
        buffer.next_read_ = 0;
        buffer.next_write_ = 0;
    }
    ~RingBuffer() = default;
    /// @brief  Enbuffer the specified event. If the cache is full, the event
    ///         will replace the oldest event in the buffer.
    /// @param event    The event to buffer
    void Enqueue(Element event) noexcept {
        std::lock_guard<Lockable> lock{mutex_};

        auto const write = next_write_ % cache_.size();
        cache_[write] = std::move(event);

        ++next_write_;
        next_read_ += (next_write_ - next_read_) > cache_.size();
    }
    /// @brief  Return the oldest event on the buffer.
    /// @return The oldest event on the buffer.
    auto Dequeue() noexcept {
        std::lock_guard<Lockable> lock{mutex_};

        if(next_read_ == next_write_) return std::optional<Element>{};

        auto const read = next_read_ % cache_.size();
        ++next_read_;

        return std::optional<Element>(std::move(cache_[read]));
    }
    /// @brief  Returns the configured capacity of the buffer.
    /// @return The configured capacity of the buffer.
    auto Capacity() const noexcept { return cache_.size(); }
    /// @brief  Returns the number of events currently bufferd.
    /// @return The number of events currently bufferd.
    auto Length() const noexcept {
        std::lock_guard<Lockable> lock{mutex_};
        return next_write_ - next_read_;
    }
    /// @brief  Indicates if the the buffer is empty
    /// @return Returns true if the buffer is empty
    auto Empty() const { return Length() == 0; }
    /// @brief  Clear the contents of the buffer
    void Clear() {
        std::lock_guard<Lockable> lock{mutex_};
        while(next_read_ != next_write_) {
            auto const read = next_read_ % cache_.size();
            cache_[read] = Element{};
            next_read_++;
        }
    }
    /// @brief Deleted
    RingBuffer& operator=(RingBuffer const&) = delete;
    //
    RingBuffer& operator=(RingBuffer&& buffer) {
        if(&buffer != this) {
            next_read_ = buffer.next_read_;
            next_write_ = buffer.next_write_;
            cache_ = buffer.cache_;
            buffer.next_read_ = 0;
            buffer.next_write_ = 0;
        }
        return *this;
    }

private:
    size_t next_read_{0};
    size_t next_write_{0};
    Cache cache_;
    mutable Lockable mutex_;
};
}