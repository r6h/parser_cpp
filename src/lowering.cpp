#include "lowering.h"
#include <string>

namespace {

bool is_block_node(const Node& node) {
    return node.type == Node::Type::Heading ||
           node.type == Node::Type::Paragraph ||
           node.type == Node::Type::CodeBlock;
}

std::string collect_inline_text(const Node& node) {
    if (node.type == Node::Type::Text) {
        return node.text;
    }
    std::string out;
    for (const auto& child : node.children) {
        out += collect_inline_text(*child);
    }
    return out;
}

std::vector<ir::Inline> lower_inlines(const std::vector<std::unique_ptr<Node>>& nodes) {
    std::vector<ir::Inline> result;

    for (const auto& child : nodes) {
        switch (child->type) {
            case Node::Type::Text: {
                if (!result.empty() && result.back().kind == ir::InlineKind::Text) {
                    result.back().text += child->text;
                } else {
                    result.push_back(ir::Inline{ir::InlineKind::Text, child->text, {}});
                }
                break;
            }
            case Node::Type::Emphasis: {
                ir::Inline inline_node{ir::InlineKind::Emphasis, {}, {}};
                inline_node.children = lower_inlines(child->children);
                result.push_back(std::move(inline_node));
                break;
            }
            case Node::Type::Strong: {
                ir::Inline inline_node{ir::InlineKind::Strong, {}, {}};
                inline_node.children = lower_inlines(child->children);
                result.push_back(std::move(inline_node));
                break;
            }
            case Node::Type::InlineCode: {
                ir::Inline inline_node{ir::InlineKind::Code, {}, {}};
                inline_node.text = collect_inline_text(*child);
                result.push_back(std::move(inline_node));
                break;
            }
            default:
                break;
        }
    }

    return result;
}

ir::Block lower_block(const Node& node) {
    ir::Block block{};

    switch (node.type) {
        case Node::Type::Heading:
            block.kind = ir::BlockKind::Heading;
            block.level = 1;
            block.content = lower_inlines(node.children);
            break;
        case Node::Type::Paragraph:
            block.kind = ir::BlockKind::Paragraph;
            block.content = lower_inlines(node.children);
            break;
        case Node::Type::CodeBlock:
            block.kind = ir::BlockKind::CodeBlock;
            block.info = node.info;
            block.text = node.text;
            break;
        default:
            break;
    }

    return block;
}

} // namespace

ir::Document lower_to_ir(const Node& node) {
    ir::Document doc;

    if (node.type != Node::Type::Document) {
        if (!is_block_node(node)) {
            return doc;
        }
        doc.blocks.push_back(lower_block(node));
        return doc;
    }

    for (const auto& child : node.children) {
        if (is_block_node(*child)) {
            doc.blocks.push_back(lower_block(*child));
        }
    }

    return doc;
}
