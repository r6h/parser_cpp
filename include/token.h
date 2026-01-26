#pragma once
#include <string_view>
#include <cstddef>

enum class TokenType {
    Hash,
    Star,
    Backtick,
    Fence,
    Text,
    Newline,
    EndOfFile
};

struct Token {
    TokenType type;
    std::string_view lexeme;
    size_t position;
};