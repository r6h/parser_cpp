#include "renderer.h"
#include <sstream>

static std::string escape_html(std::string_view text) {
    std::string out;
    out.reserve(text.size());

    for (char c : text) {
        switch (c) {
            case '&':  out += "&amp;";  break;
            case '<':  out += "&lt;";   break;
            case '>':  out += "&gt;";   break;
            case '"':  out += "&quot;"; break;
            case '\'': out += "&#39;";  break;
            default:   out += c;        break;
        }
    }

    return out;
}

static void render_inline_html(std::ostringstream& html, const ir::Inline& inline_node);

static void render_inlines_html(std::ostringstream& html, const std::vector<ir::Inline>& inlines) {
    for (const auto& node : inlines) {
        render_inline_html(html, node);
    }
}

static void render_inline_html(std::ostringstream& html, const ir::Inline& inline_node) {
    switch (inline_node.kind) {
        case ir::InlineKind::Text:
            html << escape_html(inline_node.text);
            break;
        case ir::InlineKind::Code:
            html << "<code>";
            html << escape_html(inline_node.text);
            html << "</code>\n";
            break;
        case ir::InlineKind::Emphasis:
            html << "<em>";
            render_inlines_html(html, inline_node.children);
            html << "</em>\n";
            break;
        case ir::InlineKind::Strong:
            html << "<strong>";
            render_inlines_html(html, inline_node.children);
            html << "</strong>\n";
            break;
    }
}

static void render_block_html(std::ostringstream& html, const ir::Block& block) {
    switch (block.kind) {
        case ir::BlockKind::Heading:
            html << "<h" << block.level << ">";
            render_inlines_html(html, block.content);
            html << "</h" << block.level << ">\n";
            break;
        case ir::BlockKind::Paragraph:
            html << "<p>";
            render_inlines_html(html, block.content);
            html << "</p>\n";
            break;
        case ir::BlockKind::CodeBlock:
            html << "<pre><code";
            if (!block.info.empty()) {
                html << " class=\"language-" << block.info << "\"";
            }
            html << ">\n";
            html << escape_html(block.text);
            html << "\n</code></pre>\n";
            break;
    }
}

std::string render_html(const Node& node) {
    std::ostringstream html;

    switch (node.type) {
        case Node::Type::Document:
            for (const auto& child : node.children) {
                html << render_html(*child);
            }
            break;

        case Node::Type::Heading:
            html << "<h1>";
            for (const auto& child : node.children) {
                html << render_html(*child);
            }
            html << "</h1>\n";
            break;

        case Node::Type::Paragraph:
            html << "<p>";
            for (const auto& child : node.children) {
                html << render_html(*child);
            }
            html << "</p>\n";
            break;

        case Node::Type::Text:
            html << escape_html(node.text);
            break;

        case Node::Type::InlineCode:
            html << "<code>";
            for (const auto& child : node.children) {
                html << render_html(*child);
            }
            html << "</code>\n";
            break;

        case Node::Type::Emphasis:
            html << "<em>";
            for (const auto& child : node.children) {
                html << render_html(*child);
            }
            html << "</em>\n";
            break;

        case Node::Type::Strong:
            html << "<strong>";
            for (const auto& child : node.children) {
                html << render_html(*child);
            }
            html << "</strong>\n";
            break;

        case Node::Type::CodeBlock:
            html << "<pre><code";
            if (!node.info.empty()) {
                html << " class=\"language-" << node.info << "\"";
            }
            html << ">\n";
            html << escape_html(node.text);
            html << "\n</code></pre>\n";
            break;
    }

    return html.str();
}

std::string render_html(const ir::Document& doc) {
    std::ostringstream html;

    for (const auto& block : doc.blocks) {
        render_block_html(html, block);
    }

    return html.str();
}