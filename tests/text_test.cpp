#include <menumanager/text.h>

#include <doctest/doctest.h>

#include <ostream>
#include <string>
#include <string_view>

using menumanager::decodeText;

TEST_CASE("game text to UTF-8")
{
    CHECK(decodeText("Potion") == "Potion");
    CHECK(decodeText("\x83\x41") == "\xE3\x82\xA2"); // katakana a
    CHECK(decodeText("\x1E\x02Red\x1E\x01") == "Red");
    CHECK(decodeText("\xEF\x27Hello\xEF\x28") == "{Hello}");
    CHECK(decodeText("\xFD\x02\x02\x01\x0A\xFD") == "[AT:0202010A]");
    CHECK(decodeText("one\x07two") == "one\ntwo");
}

TEST_CASE("where game text ends")
{
    CHECK(decodeText(std::string_view("ab\0cd", 5)) == "ab");
    CHECK(decodeText("ab\x83") == "ab"); // a lead byte without its second byte
    CHECK(decodeText(static_cast<const char*>(nullptr)).empty());
    CHECK(decodeText(std::string_view{}).empty());
}
