#pragma once
#include <string>
#include <vector>

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
    std::vector<Node*> children;
};