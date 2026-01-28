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

struct SourcePos {
    size_t line;
    size_t column;
};

struct Token {
    TokenType type;
    std::string_view lexeme;
    SourcePos position;
};