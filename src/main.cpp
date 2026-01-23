#include "ast.h"
#include "parser.h"
#include "renderer.h"
#include "tokenizer.h"
#include <iostream>
#include <string>

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

int main() {
  std::string input = "# Heading\nThis is a paragraph. This is `some program`, "
                      "and this *is emphasis.* \n```Now we enter a code block!```";
  Tokenizer tokenizer(input);
  Parser parser(tokenizer);
  auto document = parser.parse_document();
  print_ast(document.get());
  std::cout << render_html(*document);
  return 0;
}
