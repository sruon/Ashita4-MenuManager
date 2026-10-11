#include <menumanager/text.h>

#include <windows.h>

#include <cstddef>
#include <cstdint>
#include <format>
#include <string>
#include <string_view>

namespace menumanager
{

namespace
{

constexpr UINT kShiftJis = 932;

// A colour or other control code, followed by one operand byte
constexpr auto isControl(const uint8_t byte) -> bool
{
    return byte == 0x1E || byte == 0x1F || byte == 0x7F;
}

constexpr auto isLeadByte(const uint8_t byte) -> bool
{
    return (byte >= 0x81 && byte <= 0x9F) || (byte >= 0xE0 && byte <= 0xFC);
}

// Shift-JIS with the control codes taken out
auto stripControls(const std::string_view text) -> std::string
{
    std::string cp932;
    const auto  at = [&](const size_t index) -> uint8_t
    {
        return index < text.size() ? static_cast<uint8_t>(text[index]) : 0;
    };
    for (size_t index = 0; index < text.size() && at(index) != 0; ++index)
    {
        const auto byte = at(index);
        const auto next = at(index + 1);
        if (isControl(byte))
        {
            ++index;
        }
        else if (byte == 0xEF)
        {
            // auto-translate brackets
            if (next == 0x27 || next == 0x28)
            {
                cp932 += next == 0x27 ? '{' : '}';
            }

            ++index;
        }
        else if (byte == 0xFD)
        {
            // an auto-translate phrase, FD and four id bytes, closed by another FD
            if (index + 5 < text.size())
            {
                cp932 += std::format("[AT:{:02X}{:02X}{:02X}{:02X}]", at(index + 1), at(index + 2), at(index + 3), at(index + 4));
            }

            index += 5;
        }
        else if (byte == 0x07 || byte == 0x0A)
        {
            cp932 += '\n';
        }
        else if (byte < 0x20)
        {
            continue;
        }
        else if (isLeadByte(byte))
        {
            if (next == 0)
            {
                break;
            }

            cp932 += static_cast<char>(byte);
            cp932 += static_cast<char>(next);
            ++index;
        }
        else
        {
            cp932 += static_cast<char>(byte);
        }
    }

    return cp932;
}

} // namespace

auto decodeText(const std::string_view cp932) -> std::string
{
    const auto text = stripControls(cp932);
    if (text.empty())
    {
        return {};
    }

    const auto   wideLength = ::MultiByteToWideChar(kShiftJis, 0, text.data(), static_cast<int>(text.size()), nullptr, 0);
    std::wstring wide(static_cast<size_t>(wideLength), L'\0');
    ::MultiByteToWideChar(kShiftJis, 0, text.data(), static_cast<int>(text.size()), wide.data(), wideLength);

    const auto  utf8Length = ::WideCharToMultiByte(CP_UTF8, 0, wide.data(), wideLength, nullptr, 0, nullptr, nullptr);
    std::string utf8(static_cast<size_t>(utf8Length), '\0');
    ::WideCharToMultiByte(CP_UTF8, 0, wide.data(), wideLength, utf8.data(), utf8Length, nullptr, nullptr);
    return utf8;
}

auto decodeText(const char* cp932) -> std::string
{
    return cp932 == nullptr ? std::string{} : decodeText(std::string_view(cp932));
}

} // namespace menumanager
