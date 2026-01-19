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

    Node* parse_document();
    Node* parse_block();
    Node* parse_heading();
    Node* parse_paragraph();

private:
    Tokenizer& tokenizer_;
    Token current_;

    void advance();
};