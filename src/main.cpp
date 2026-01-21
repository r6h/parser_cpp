#include <iostream>
#include <string>
#include "tokenizer.h"
#include "parser.h"
#include "ast.h"

// little helper to convert enum to text for better debugging
std::string node_type_to_string(Node::Type type) {
    switch (type) {
        case Node::Type::Document:    return "Document";
        case Node::Type::Heading:     return "Heading";
        case Node::Type::Paragraph:   return "Paragraph";
        case Node::Type::Text:        return "Text";
        case Node::Type::Emphasis:    return "Emphasis";
        case Node::Type::InlineCode:  return "InlineCode";
    }
    return "Unknown";
}

void print_ast(Node* node, const std::string& prefix = "", bool is_last = true) {
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
        bool last = (i == node->children.size() -1);
        print_ast(node->children[i], new_prefix, last);
    }
}

void free_ast(Node* node) {
    for (Node* child : node->children) {
        free_ast(child);
    }
    delete node;
}

int main() {
    std::string input = "# Heading\nThis is a paragraph. This is `code`";
    Tokenizer tokenizer(input);
    Parser parser(tokenizer);
    Node* ast = parser.parse_document();
    print_ast(ast);
    free_ast(ast);
    return 0;
}