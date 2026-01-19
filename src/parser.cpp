#include "parser.h"
#include "ast.h"
#include <cstdlib>

Node* Parser::parse_document() {
  Node *document = new Node{Node::Type::Document};

  while (current_.type != TokenType::EndOfFile) {
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

Node* Parser::parse_heading() {
  Node *heading = new Node{Node::Type::Heading};

  advance(); // consume '#'

  // parse inline content (currently just text)
  if (current_.type == TokenType::Text) {
    Node *text = new Node{Node::Type::Text};
    text->text = current_.lexeme;

    heading->children.push_back(text);
    advance();
  }

  return heading;
}

Node* Parser::parse_paragraph() {
  Node* paragraph = new Node{Node::Type::Paragraph};

  // consume tokens until newline or EOF
  while (current_.type != TokenType::Newline &&
          current_.type != TokenType::EndOfFile) {
        
        // just handling text for now
        if (current_.type == TokenType::Text) {
          Node* text = new Node{Node::Type::Text};
          text->text = current_.lexeme;
          
          paragraph->children.push_back(text); 
        }
        advance();
      }

      if (current_.type == TokenType::Newline) {
        advance();
      }

      return paragraph;
}

void Parser::advance() {
    current_ = tokenizer_.next();
}