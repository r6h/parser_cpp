#pragma once
#include <memory>
#include <string>
#include <vector>

struct Node {
  enum class Type {
    Document,
    Paragraph,
    Heading,
    Text,
    Emphasis,
    InlineCode,
    Strong,
    CodeBlock
  };

  Type type;

  std::string text;
  std::string code;
  std::string info;

  std::vector<std::unique_ptr<Node>> children;

  explicit Node(Type t) : type(t) {}
};
