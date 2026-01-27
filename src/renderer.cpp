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