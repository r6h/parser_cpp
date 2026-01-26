#pragma once
#include "token.h"
#include <string>

class Tokenizer {
    public:
        explicit Tokenizer(const std::string& input)
            : input_(input), pos_(0) {}

        Token next();
        Token peek(size_t n);

    private:
        const std::string& input_;
        size_t pos_;
        bool is_fence_start(size_t pos) const;
};