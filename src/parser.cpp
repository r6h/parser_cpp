#include "parser.h"
#include "ast.h"
#include <cstdlib>
#include <cassert>

void Parser::parse_inlines_into(std::vector<std::unique_ptr<Node>> &out,
                                TokenType stop) {
  while (current_.type != stop && current_.type != TokenType::EndOfFile) {
    auto node = parse_inline();

    if (node) {
      out.push_back(std::move(node));
    } else {
      // if parse_inline returns nullptr, advance to avoid infinite loop
      advance();
    }
  }
}

std::unique_ptr<Node> Parser::parse_document() {
  auto document = std::make_unique<Node>(Node::Type::Document);

  while (current_.type != TokenType::EndOfFile) {
    // skip newlines between blocks
    while (current_.type == TokenType::Newline) {
      advance();
    }
    if (current_.type == TokenType::EndOfFile)
      break;

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

  if (current_.type == TokenType::Backtick &&
      tokenizer_.peek(1).type == TokenType::Backtick &&
      tokenizer_.peek(2).type == TokenType::Backtick) {
        return parse_code_block();
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

  if (current_.type == TokenType::Star &&
      tokenizer_.peek(1).type == TokenType::Star) {
    advance();
    advance();
    auto strong = std::make_unique<Node>(Node::Type::Strong);
    
    while (!(current_.type == TokenType::Star && tokenizer_.peek(1).type == TokenType::Star) &&
           current_.type != TokenType::EndOfFile && current_.type != TokenType::Newline) {
      auto child = parse_inline();
      if (child) {
        strong->children.push_back(std::move(child));
      } else {
        break;
      }
    }

    if (current_.type == TokenType::Star && tokenizer_.peek(1).type == TokenType::Star) {
      advance();
      advance();
    }
    return strong;
  }

  if (current_.type == TokenType::Star) {
    advance();
    auto emphasis = std::make_unique<Node>(Node::Type::Emphasis);
    
    while (current_.type != TokenType::Star &&
           current_.type != TokenType::EndOfFile &&
           current_.type != TokenType::Newline) {
      auto child = parse_inline();
      if (child) {
        emphasis->children.push_back(std::move(child));
      } else {
        break;
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

  // don't advance here, let the caller handle unknown tokens
  return nullptr;
}

std::unique_ptr<Node> Parser::parse_heading() {
  auto heading = std::make_unique<Node>(Node::Type::Heading);

  advance();

  // skip exactly one optional space after the hash for proper markdown rules
  if (current_.type == TokenType::Text && !current_.lexeme.empty() &&
      current_.lexeme.front() == ' ') {
    current_.lexeme.remove_prefix(1);
  }

  parse_inlines_into(heading->children, TokenType::Newline);

  if (current_.type == TokenType::Newline) {
    advance();
  }

  return heading;
}

std::unique_ptr<Node> Parser::parse_paragraph() {
  auto paragraph = std::make_unique<Node>(Node::Type::Paragraph);

  parse_inlines_into(paragraph->children, TokenType::Newline);

  if (current_.type == TokenType::Newline) {
    advance();
  }

  return paragraph;
}

std::unique_ptr<Node> Parser::parse_code_block() {
  auto block = std::make_unique<Node>(Node::Type::CodeBlock);

  advance();
  advance();
  advance();

  if (current_.type == TokenType::Newline)
    advance();
  
  while (!(current_.type == TokenType::Backtick &&
          tokenizer_.peek(1).type == TokenType::Backtick &&
          tokenizer_.peek(2).type == TokenType::Backtick)) {

            if (current_.type == TokenType::EndOfFile)
              break;

            block->text += current_.lexeme;
            advance();
          }
  
  advance();
  advance();
  advance();

  if (current_.type == TokenType::Newline)
    advance();

  return block;
}

void Parser::advance() {
  current_ = tokenizer_.next();
}
