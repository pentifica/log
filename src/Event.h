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
#include    "Severity.h"

#include    <iostream>
#include    <chrono>
#include    <memory>

namespace pentifica::log { class Event; }
std::ostream& operator<<(std::ostream&, pentifica::log::Event const&);

namespace pentifica::log {
/// @brief  Represents the common information all logging events will
///         capture.
class Event {
public:
    using Clock = std::chrono::system_clock;
    using TimePoint = Clock::time_point;
    Event() = default;
    /// @brief  Initialize instance using the indicated Severity
    /// @param severity Specifies the Event severity
    explicit Event(Severity severity) : severity_{severity} {}
    /// @brief  Initialize instance setting bot the Severity and the event time
    /// @param severity Specifies the Event severity
    /// @param time     Specifies the event time
    explicit Event(Severity severity, TimePoint time) : severity_{severity}, time_{time} {}
    Event(Event const&) = default;
    Event(Event&&) = default;
    virtual ~Event() = default;
    /// @brief  Reset the Severity associated with the Event
    /// @param severty  The Severity update
    void Reset(Severity severty) { severity_ = severty; }
    /// @brief  Reset the Event time
    /// @param time The Event time update
    void Reset(TimePoint time) { time_ = time; }
    Event& operator=(Event const&) = default;
    Event& operator=(Event&&) = default;
    /// @brief  Stream the Event information to the indicated stream
    /// @param  os      Where to stream the Event state
    /// @param  event   The Event to stream
    /// @return The supplied stream
    friend std::ostream& ::operator<<(std::ostream& os, Event const& event);

protected:
    /// @brief  Support for streaming derived classes
    /// @param  os  Where to stream derived state
    virtual void Log(std::ostream& os) const = 0;

private:
    Severity severity_ {Severity::Debug};
    TimePoint time_ {Clock::now()};
};

using EventDel = void(*)(Event*);

using EventRef = std::unique_ptr<Event, EventDel>;
}