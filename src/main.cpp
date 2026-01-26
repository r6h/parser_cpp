#include "ast.h"
#include "parser.h"
#include "renderer.h"
#include "tokenizer.h"
#include <iostream>
#include <string>
#include <cassert>

// little helper to convert enum to text for better debugging
std::string node_type_to_string(Node::Type type) {
  switch (type) {
  case Node::Type::Document:
    return "Document";
  case Node::Type::Heading:
    return "Heading";
  case Node::Type::Paragraph:
    return "Paragraph";
  case Node::Type::Text:
    return "Text";
  case Node::Type::Emphasis:
    return "Emphasis";
  case Node::Type::InlineCode:
    return "InlineCode";
  case Node::Type::Strong:
    return "Strong";
  case Node::Type::CodeBlock:
    return "CodeBlock";
  }
  return "Unknown";
}

void print_ast(Node *node, const std::string &prefix = "",
               bool is_last = true) {
  std::cout << prefix;

  if (!prefix.empty()) {
    std::cout << (is_last ? "└── " : "├── ");
  }

  std::cout << node_type_to_string(node->type);

  if (node->type == Node::Type::Text) {
    std::cout << "(\"" << node->text << "\")";
  }

  std::cout << "\n";

  std::string new_prefix = prefix + (is_last ? "    " : "|   ");

  for (size_t i = 0; i < node->children.size(); ++i) {
    print_ast(node->children[i].get(), new_prefix,
              i + 1 == node->children.size());
  }
}

// format for peek test output
const char* token_type_to_string(TokenType t) {
  switch (t) {
    case TokenType::Hash: return "Hash";
    case TokenType::Star: return "Star";
    case TokenType::Text: return "Text";
    case TokenType::EndOfFile: return "EOF";
    case TokenType::Backtick: return "Backtick";
    case TokenType::Newline: return "Newline";
    }
    return "Unknown";
}

// tests, temporarily in main but to be moved to tests/

void test_peek() {
  std::string input = "* *";
  Tokenizer t(input);

  (void)t.next();

  Token peeked = t.peek(1);
  Token after = t.next(); // should be the same as peeked

  std::cout << token_type_to_string(peeked.type) << "\n";
  std::cout << "peek type: " << (int)peeked.type << "\n";
  std::cout << "next type: " << (int)after.type << "\n";
}

void test_peek_does_not_advance() {
    Tokenizer t("abc");

    (void)t.next();
    Token b1 = t.peek(1);
    Token b2 = t.peek(1);
    Token b3 = t.next();

    assert(b1.type == b2.type);
    assert(b1.type == b3.type);
}

void test_html_escaping() {
    Node n(Node::Type::Text);
    n.text = "<>&\"'";

    std::string out = render_html(n);
    assert(out == "&lt;&gt;&amp;&quot;&#39;");
}


int main() {
  test_peek();
  test_peek_does_not_advance();

  std::string input = "# Heading\nThis is a paragraph. This is `some program`, "
                      "and this *is emphasis.* \n```Now we enter a code block!```\n**bold text**";
  Tokenizer tokenizer(input);
  Parser parser(tokenizer);
  auto document = parser.parse_document();
  print_ast(document.get());
  std::cout << render_html(*document);
  return 0;
}
