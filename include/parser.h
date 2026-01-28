#pragma once
#include "token.h"
#include "tokenizer.h"
#include "ast.h"
#include <optional>

struct ParseError {
    std::string message;
    SourcePos pos;
};

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
    std::unique_ptr<Node> parse_code_block();

    bool has_error() const {
        return error_.has_value();
    }
    const ParseError& get_error() const {
        return error_.value();
    }

private:
    Tokenizer& tokenizer_;
    Token current_;
    std::optional<ParseError> error_;

    void advance();
    void error(const std::string& msg);

    void parse_inlines_into(std::vector<std::unique_ptr<Node>>& out,
                            TokenType stop);
};