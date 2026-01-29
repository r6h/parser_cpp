#pragma once
#include <string>
#include <vector>

namespace ir {

enum class BlockKind {
    Heading,
    Paragraph,
    CodeBlock
};

enum class InlineKind {
    Text,
    Emphasis,
    Strong,
    Code
};

struct Inline {
    InlineKind kind;
    std::string text;
    std::vector<Inline> children;
};

struct Block {
    BlockKind kind;
    int level = 0; // headings only
    std::string info; // code block info string
    std::string text; // code block text
    std::vector<Inline> content; // heading/paragraph inlines
};

struct Document {
    std::vector<Block> blocks;
};

} // namespace ir
