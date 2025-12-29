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

private:
    Tokenizer& tokenizer_;
    Token current_;

    void advance();
}

void Parser::advance() {
    current_ = tokenizer_.next();
}