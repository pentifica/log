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
#include    <cstdint>

/// @brief Defines the logging severity levels
namespace pentifica::log {
    enum Severity : std::uint8_t {
        Debug,
        Info,
        Tracking,
        Logic,
        Critical,
        Alert,
        Fatal,
    };

    /// @brief Translates a severity level to a human readable string
    /// @param severity The severity level to translate
    /// @return A pointer to a human readable string representing the severity
    ///         level provided
    constexpr char const* const ToString(Severity severity) noexcept {
        switch(severity) {
            case Severity::Debug:       return "Debug   ";
            case Severity::Info:        return "Info    ";
            case Severity::Tracking:    return "Tracking";
            case Severity::Logic:       return "Logic   ";
            case Severity::Critical:    return "Critical";
            case Severity::Alert:       return "Alert   ";
            case Severity::Fatal:       return "Fatal   ";
            default:                    return "Unknown ";
        }
    }
}