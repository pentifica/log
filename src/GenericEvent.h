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
#include <Factory.h>
#include <Utility.h>

#include <tuple>

namespace pentifica::log {
/// @brief  Defines a generic Event class that can be instantiated with any value
///         types.
/// @tparam ...Fields   Parameter pack of fields the generic event will capture.
template<typename... Fields>
class GenericEvent :
    public Event
{
    using TupleType = std::tuple<Fields...>;
public:
    using Event::Event;
    GenericEvent(Fields... fields) : data_{std::make_tuple(fields...)} {}
    virtual ~GenericEvent() = default;
    /// @brief  Streams the contained value types without any formatting
    ///         assumption
    /// @param os   Where to stream the values
    void Log(std::ostream& os) const override {
        os << data_;
    }

private:
    TupleType data_;
};
}