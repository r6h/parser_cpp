#include "parser.h"
#include "ast.h"
#include <cstdlib>

Node* Parser::parse_document() {
  Node *document = new Node{Node::Type::Document};

  while (current_.type != TokenType::EndOfFile) {
      // skip newlines between blocks
      while (current_.type == TokenType::Newline) {
        advance();
      }
      if (current_.type == TokenType::EndOfFile) break;

    Node *block = parse_block();
    document->children.push_back(block);
  }

  return document;
}

Node* Parser::parse_block() {
  if (current_.type == TokenType::Hash) {
    return parse_heading();
  }

  // temp fallback
  return parse_paragraph();
}

Node* Parser::parse_inline() {
  if (current_.type == TokenType::Text) {
    Node* text = new Node{Node::Type::Text};
    text->text = current_.lexeme;
    advance();
    return text;
  }

  if (current_.type == TokenType::Star) {
    advance();

    Node* emphasis = new Node{Node::Type::Emphasis};

    while (current_.type != TokenType::Star &&
          current_.type != TokenType::Newline &&
          current_.type != TokenType::EndOfFile) {

            // recursively parse inside emphasis
            Node* child = parse_inline();
            if (child) {
              emphasis->children.push_back(child);
            }
          }
    

    if (current_.type == TokenType::Star) {
      advance();
    }

    return emphasis;
  }

  if (current_.type == TokenType::Backtick) {
    advance();
    Node* code = new Node{Node::Type::InlineCode};

    if (current_.type == TokenType::Text) {
      Node* child = new Node{Node::Type::Text};
      child->text = current_.lexeme;
      code->children.push_back(child);
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

Node* Parser::parse_heading() {
  Node *heading = new Node{Node::Type::Heading};

  advance();
   
  // skip exactly one optional space after the hash for proper markdown rules
  if (current_.type == TokenType::Text &&
      !current_.lexeme.empty() &&
      current_.lexeme.front() == ' ') {
        current_.lexeme.remove_prefix(1);
      }

  while (current_.type != TokenType::Newline &&
          current_.type != TokenType::EndOfFile) {

            Node* child = parse_inline();
            if (child) {
              heading->children.push_back(child);
            }
  }
  if (current_.type == TokenType::Newline) {
    advance();
  }

  return heading;
}

Node* Parser::parse_paragraph() {
  Node* paragraph = new Node{Node::Type::Paragraph};

  // consume tokens until newline or EOF
  while (current_.type != TokenType::Newline &&
          current_.type != TokenType::EndOfFile) {
        
        Node* child = parse_inline();
        if (child) {
          paragraph->children.push_back(child);
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