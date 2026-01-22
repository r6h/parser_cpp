#pragma once
#include "token.h"
#include "tokenizer.h"
#include "ast.h"

class Parser {
public:
    explicit Parser(Tokenizer& tokenizer)
    : tokenizer_(tokenizer) {
        current_ = tokenizer_.next();
    }

    std::unique_ptr<Node> parse_document();
    std::unique_ptr<Node> parse_block();
    std::unique_ptr<Node> parse_heading();
    std::unique_ptr<Node> parse_paragraph();
    std::unique_ptr<Node> parse_inline();

private:
    Tokenizer& tokenizer_;
    Token current_;

    void advance();
};