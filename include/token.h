#pragma once
#include <string_view>
#include <cstdef>

enum class TokenType {
    Hash,
    Star,
    Backtick,
    Text,
    Newline,
    EndOfFile
};

struct Token {
    TokenType type;
    std::string_view lexeme;
    size_t position;
};