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
#include    "Event.h"

#include    <iostream>
#include    <iomanip>
#include    <ctime>

std::ostream& operator<<(std::ostream& os, pentifica::log::Event const& event) {
    using Clock = pentifica::log::Event::Clock;

    auto clock_time = Clock::to_time_t(event.time_);
    std::tm tm{0};
    localtime_r(&clock_time, &tm);

    char mask[] = "2000-00-00 00:00:00.";

    mask[2] += (tm.tm_year - 100) / 10;
    mask[3] += tm.tm_year % 10;

    tm.tm_mon += 1;
    mask[5] += tm.tm_mon / 10;
    mask[6] += tm.tm_mon % 10;

    mask[8] += tm.tm_mday / 10;
    mask[9] += tm.tm_mday % 10;

    mask[11] += tm.tm_hour / 10;
    mask[12] += tm.tm_hour % 10;

    mask[14] += tm.tm_min / 10;
    mask[15] += tm.tm_min % 10;

    mask[17] += tm.tm_sec / 10;
    mask[18] += tm.tm_sec % 10;

    auto frac = event.time_ - Clock::from_time_t(clock_time);
    auto microseconds = frac / std::chrono::microseconds(1);

    os << mask
       << std::setw(6) << std::setfill('0') << microseconds
       << " [" << pentifica::log::ToString(event.severity_) << "] ";

    event.Log(os);
    return os << '\n';
}