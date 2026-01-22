#include "parser.h"
#include "ast.h"
#include <cstdlib>

std::unique_ptr<Node> Parser::parse_document() {
  auto document = std::make_unique<Node>(Node::Type::Document);

  while (current_.type != TokenType::EndOfFile) {
      // skip newlines between blocks
      while (current_.type == TokenType::Newline) {
        advance();
      }
      if (current_.type == TokenType::EndOfFile) break;

    auto block = parse_block();
    if (block) {
      document->children.push_back(std::move(block));
    }
  }

  return document;
}

std::unique_ptr<Node> Parser::parse_block() {
  if (current_.type == TokenType::Hash) {
    return parse_heading();
  }

  // temp fallback
  return parse_paragraph();
}

std::unique_ptr<Node> Parser::parse_inline() {
  if (current_.type == TokenType::Text) {
    auto text = std::make_unique<Node>(Node::Type::Text);
    text->text = current_.lexeme;
    advance();
    return text;
  }

  if (current_.type == TokenType::Star) {
    advance();

    auto emphasis = std::make_unique<Node>(Node::Type::Emphasis);

    while (current_.type != TokenType::Star &&
          current_.type != TokenType::Newline &&
          current_.type != TokenType::EndOfFile) {

            auto child = parse_inline();
            if (child) {
              emphasis->children.push_back(std::move(child));
            }
          }

    if (current_.type == TokenType::Star) {
      advance();
    }

    return emphasis;
  }

  if (current_.type == TokenType::Backtick) {
    advance();
    auto code = std::make_unique<Node>(Node::Type::InlineCode);

    if (current_.type == TokenType::Text) {
      auto child = std::make_unique<Node>(Node::Type::Text);
      child->text = current_.lexeme;
      code->children.push_back(std::move(child));
      advance();
    }

    if (current_.type == TokenType::Backtick) {
      advance();
    }

    return code;
  }

  advance();
  return nullptr;
}

std::unique_ptr<Node> Parser::parse_heading() {
  auto heading = std::make_unique<Node>(Node::Type::Heading);

  advance();

  // skip exactly one optional space after the hash for proper markdown rules
  if (current_.type == TokenType::Text &&
      !current_.lexeme.empty() &&
      current_.lexeme.front() == ' ') {
        current_.lexeme.remove_prefix(1);
      }

  while (current_.type != TokenType::Newline &&
          current_.type != TokenType::EndOfFile) {

            auto child = parse_inline();
            if (child) {
              heading->children.push_back(std::move(child));
            }
  }
  if (current_.type == TokenType::Newline) {
    advance();
  }

  return heading;
}

std::unique_ptr<Node> Parser::parse_paragraph() {
  auto paragraph = std::make_unique<Node>(Node::Type::Paragraph);

  // consume tokens until newline or EOF
  while (current_.type != TokenType::Newline &&
          current_.type != TokenType::EndOfFile) {

        auto child = parse_inline();
        if (child) {
          paragraph->children.push_back(std::move(child));
        }
  }

  if (current_.type == TokenType::Newline) {
    advance();
  }

  return paragraph;
}

void Parser::advance() {
    current_ = tokenizer_.next();
}