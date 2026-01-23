#include "renderer.h"
#include <sstream>

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
            html << node.text;
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
            html << "<pre><code>";
            html << node.text;
            html << "</code></pre>\n";
            break;
    }

    return html.str();
}