#pragma once
#include <string>
#include <vector>
#include <memory>

struct Node {
    enum class Type {
        Document,
        Paragraph,
        Heading,
        Text,
        Emphasis,
        InlineCode
    };

    Type type;
    std::string text;
    std::vector<std::unique_ptr<Node>> children;

    explicit Node(Type t) : type(t) {}
};