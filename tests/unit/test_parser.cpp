#include <cassert>
#include <iostream>
#include <string>
#include <memory>
#include "tokenizer.h"
#include "parser.h"
#include "ast.h"

// Helper to check node type
void assert_node_type(const Node* node, Node::Type expected, const std::string& test_name) {
    if (!node) {
        std::cerr << "FAIL: " << test_name << " - Node is null\n";
        std::exit(1);
    }
    if (node->type != expected) {
        std::cerr << "FAIL: " << test_name << " - Expected node type mismatch\n";
        std::exit(1);
    }
}

// Helper to check child count
void assert_child_count(const Node* node, size_t expected, const std::string& test_name) {
    if (!node) {
        std::cerr << "FAIL: " << test_name << " - Node is null\n";
        std::exit(1);
    }
    if (node->children.size() != expected) {
        std::cerr << "FAIL: " << test_name << " - Expected " << expected
                  << " children but got " << node->children.size() << "\n";
        std::exit(1);
    }
}

// Helper to check text content
void assert_text(const Node* node, const std::string& expected, const std::string& test_name) {
    if (!node) {
        std::cerr << "FAIL: " << test_name << " - Node is null\n";
        std::exit(1);
    }
    if (node->text != expected) {
        std::cerr << "FAIL: " << test_name << " - Expected text '" << expected
                  << "' but got '" << node->text << "'\n";
        std::exit(1);
    }
}

// ==================== Document Tests ====================

void test_empty_document() {
    std::string input = "";
    Tokenizer tokenizer(input);
    Parser parser(tokenizer);
    auto doc = parser.parse_document();
    
    assert_node_type(doc.get(), Node::Type::Document, "test_empty_document");
    assert_child_count(doc.get(), 0, "test_empty_document");
    
    std::cout << "[OK] test_empty_document\n";
}

void test_whitespace_only_document() {
    std::string input = "\n\n\n";
    Tokenizer tokenizer(input);
    Parser parser(tokenizer);
    auto doc = parser.parse_document();
    
    assert_node_type(doc.get(), Node::Type::Document, "test_whitespace_only_document");
    assert_child_count(doc.get(), 0, "test_whitespace_only_document");
    
    std::cout << "[OK] test_whitespace_only_document\n";
}

// ==================== Heading Tests ====================

void test_simple_heading() {
    std::string input = "# Hello";
    Tokenizer tokenizer(input);
    Parser parser(tokenizer);
    auto doc = parser.parse_document();
    
    assert_node_type(doc.get(), Node::Type::Document, "test_simple_heading doc");
    assert_child_count(doc.get(), 1, "test_simple_heading doc children");
    
    const Node* heading = doc->children[0].get();
    assert_node_type(heading, Node::Type::Heading, "test_simple_heading heading");
    assert_child_count(heading, 1, "test_simple_heading heading children");
    
    const Node* text = heading->children[0].get();
    assert_node_type(text, Node::Type::Text, "test_simple_heading text");
    assert_text(text, "Hello", "test_simple_heading text content");
    
    std::cout << "[OK] test_simple_heading\n";
}

void test_heading_with_trailing_newline() {
    std::string input = "# Hello\n";
    Tokenizer tokenizer(input);
    Parser parser(tokenizer);
    auto doc = parser.parse_document();
    
    assert_child_count(doc.get(), 1, "test_heading_with_trailing_newline");
    
    const Node* heading = doc->children[0].get();
    assert_node_type(heading, Node::Type::Heading, "test_heading_with_trailing_newline");
    
    std::cout << "[OK] test_heading_with_trailing_newline\n";
}

// ==================== Paragraph Tests ====================

void test_simple_paragraph() {
    std::string input = "Hello world";
    Tokenizer tokenizer(input);
    Parser parser(tokenizer);
    auto doc = parser.parse_document();
    
    assert_child_count(doc.get(), 1, "test_simple_paragraph doc");
    
    const Node* para = doc->children[0].get();
    assert_node_type(para, Node::Type::Paragraph, "test_simple_paragraph");
    assert_child_count(para, 1, "test_simple_paragraph children");
    
    const Node* text = para->children[0].get();
    assert_node_type(text, Node::Type::Text, "test_simple_paragraph text");
    assert_text(text, "Hello world", "test_simple_paragraph text content");
    
    std::cout << "[OK] test_simple_paragraph\n";
}

void test_two_paragraphs() {
    std::string input = "First\n\nSecond";
    Tokenizer tokenizer(input);
    Parser parser(tokenizer);
    auto doc = parser.parse_document();
    
    assert_child_count(doc.get(), 2, "test_two_paragraphs doc");
    
    const Node* para1 = doc->children[0].get();
    assert_node_type(para1, Node::Type::Paragraph, "test_two_paragraphs para1");
    
    const Node* para2 = doc->children[1].get();
    assert_node_type(para2, Node::Type::Paragraph, "test_two_paragraphs para2");
    
    std::cout << "[OK] test_two_paragraphs\n";
}

// ==================== Emphasis Tests ====================

void test_emphasis() {
    std::string input = "*hello*";
    Tokenizer tokenizer(input);
    Parser parser(tokenizer);
    auto doc = parser.parse_document();
    
    assert_child_count(doc.get(), 1, "test_emphasis doc");
    
    const Node* para = doc->children[0].get();
    assert_node_type(para, Node::Type::Paragraph, "test_emphasis para");
    assert_child_count(para, 1, "test_emphasis para children");
    
    const Node* em = para->children[0].get();
    assert_node_type(em, Node::Type::Emphasis, "test_emphasis em");
    assert_child_count(em, 1, "test_emphasis em children");
    
    const Node* text = em->children[0].get();
    assert_node_type(text, Node::Type::Text, "test_emphasis text");
    assert_text(text, "hello", "test_emphasis text content");
    
    std::cout << "[OK] test_emphasis\n";
}

void test_emphasis_in_sentence() {
    std::string input = "This is *important* text";
    Tokenizer tokenizer(input);
    Parser parser(tokenizer);
    auto doc = parser.parse_document();
    
    const Node* para = doc->children[0].get();
    assert_node_type(para, Node::Type::Paragraph, "test_emphasis_in_sentence");
    assert_child_count(para, 3, "test_emphasis_in_sentence children");
    
    assert_node_type(para->children[0].get(), Node::Type::Text, "test_emphasis_in_sentence t1");
    assert_node_type(para->children[1].get(), Node::Type::Emphasis, "test_emphasis_in_sentence em");
    assert_node_type(para->children[2].get(), Node::Type::Text, "test_emphasis_in_sentence t2");
    
    std::cout << "[OK] test_emphasis_in_sentence\n";
}

// ==================== Strong Tests ====================

void test_strong() {
    std::string input = "**bold**";
    Tokenizer tokenizer(input);
    Parser parser(tokenizer);
    auto doc = parser.parse_document();
    
    assert_child_count(doc.get(), 1, "test_strong doc");
    
    const Node* para = doc->children[0].get();
    assert_child_count(para, 1, "test_strong para children");
    
    const Node* strong = para->children[0].get();
    assert_node_type(strong, Node::Type::Strong, "test_strong strong");
    assert_child_count(strong, 1, "test_strong strong children");
    
    const Node* text = strong->children[0].get();
    assert_node_type(text, Node::Type::Text, "test_strong text");
    assert_text(text, "bold", "test_strong text content");
    
    std::cout << "[OK] test_strong\n";
}

void test_strong_in_sentence() {
    std::string input = "This is **very** important";
    Tokenizer tokenizer(input);
    Parser parser(tokenizer);
    auto doc = parser.parse_document();
    
    const Node* para = doc->children[0].get();
    assert_child_count(para, 3, "test_strong_in_sentence children");
    
    assert_node_type(para->children[0].get(), Node::Type::Text, "test_strong_in_sentence t1");
    assert_node_type(para->children[1].get(), Node::Type::Strong, "test_strong_in_sentence strong");
    assert_node_type(para->children[2].get(), Node::Type::Text, "test_strong_in_sentence t2");
    
    std::cout << "[OK] test_strong_in_sentence\n";
}

// ==================== Inline Code Tests ====================

void test_inline_code() {
    std::string input = "`code`";
    Tokenizer tokenizer(input);
    Parser parser(tokenizer);
    auto doc = parser.parse_document();
    
    const Node* para = doc->children[0].get();
    assert_child_count(para, 1, "test_inline_code para children");
    
    const Node* code = para->children[0].get();
    assert_node_type(code, Node::Type::InlineCode, "test_inline_code code");
    assert_child_count(code, 1, "test_inline_code code children");
    
    std::cout << "[OK] test_inline_code\n";
}

void test_inline_code_in_sentence() {
    std::string input = "Use `printf` function";
    Tokenizer tokenizer(input);
    Parser parser(tokenizer);
    auto doc = parser.parse_document();
    
    const Node* para = doc->children[0].get();
    assert_child_count(para, 3, "test_inline_code_in_sentence children");
    
    assert_node_type(para->children[0].get(), Node::Type::Text, "test_inline_code_in_sentence t1");
    assert_node_type(para->children[1].get(), Node::Type::InlineCode, "test_inline_code_in_sentence code");
    assert_node_type(para->children[2].get(), Node::Type::Text, "test_inline_code_in_sentence t2");
    
    std::cout << "[OK] test_inline_code_in_sentence\n";
}

// ==================== Code Block Tests ====================

void test_code_block_simple() {
    std::string input = "```\ncode\n```";
    Tokenizer tokenizer(input);
    Parser parser(tokenizer);
    auto doc = parser.parse_document();
    
    assert_child_count(doc.get(), 1, "test_code_block_simple doc");
    
    const Node* block = doc->children[0].get();
    assert_node_type(block, Node::Type::CodeBlock, "test_code_block_simple");
    assert(block->info.empty());
    
    std::cout << "[OK] test_code_block_simple\n";
}

void test_code_block_with_language() {
    std::string input = "```cpp\nint x = 42;\n```";
    Tokenizer tokenizer(input);
    Parser parser(tokenizer);
    auto doc = parser.parse_document();
    
    assert_child_count(doc.get(), 1, "test_code_block_with_language doc");
    
    const Node* block = doc->children[0].get();
    assert_node_type(block, Node::Type::CodeBlock, "test_code_block_with_language");
    assert(block->info == "cpp");
    
    std::cout << "[OK] test_code_block_with_language\n";
}

void test_code_block_multiline() {
    std::string input = "```js\nconst x = 1;\nconst y = 2;\n```";
    Tokenizer tokenizer(input);
    Parser parser(tokenizer);
    auto doc = parser.parse_document();
    
    const Node* block = doc->children[0].get();
    assert_node_type(block, Node::Type::CodeBlock, "test_code_block_multiline");
    assert(block->info == "js");
    // Check that text contains both lines
    assert(block->text.find("const x = 1;") != std::string::npos);
    assert(block->text.find("const y = 2;") != std::string::npos);
    
    std::cout << "[OK] test_code_block_multiline\n";
}

// ==================== Mixed Content Tests ====================

void test_heading_then_paragraph() {
    std::string input = "# Title\n\nSome text";
    Tokenizer tokenizer(input);
    Parser parser(tokenizer);
    auto doc = parser.parse_document();
    
    assert_child_count(doc.get(), 2, "test_heading_then_paragraph doc");
    
    assert_node_type(doc->children[0].get(), Node::Type::Heading, "test_heading_then_paragraph h");
    assert_node_type(doc->children[1].get(), Node::Type::Paragraph, "test_heading_then_paragraph p");
    
    std::cout << "[OK] test_heading_then_paragraph\n";
}

void test_paragraph_then_code_block() {
    std::string input = "Some text\n\n```\ncode\n```";
    Tokenizer tokenizer(input);
    Parser parser(tokenizer);
    auto doc = parser.parse_document();
    
    assert_child_count(doc.get(), 2, "test_paragraph_then_code_block doc");
    
    assert_node_type(doc->children[0].get(), Node::Type::Paragraph, "test_paragraph_then_code_block p");
    assert_node_type(doc->children[1].get(), Node::Type::CodeBlock, "test_paragraph_then_code_block cb");
    
    std::cout << "[OK] test_paragraph_then_code_block\n";
}

void test_complex_document() {
    std::string input = "# Heading\n\nA *paragraph* with **formatting**.\n\n```cpp\ncode\n```";
    Tokenizer tokenizer(input);
    Parser parser(tokenizer);
    auto doc = parser.parse_document();
    
    assert_child_count(doc.get(), 3, "test_complex_document doc");
    
    assert_node_type(doc->children[0].get(), Node::Type::Heading, "test_complex_document h");
    assert_node_type(doc->children[1].get(), Node::Type::Paragraph, "test_complex_document p");
    assert_node_type(doc->children[2].get(), Node::Type::CodeBlock, "test_complex_document cb");
    
    // Check paragraph has mixed content
    const Node* para = doc->children[1].get();
    bool hasEmphasis = false;
    bool hasStrong = false;
    for (const auto& child : para->children) {
        if (child->type == Node::Type::Emphasis) hasEmphasis = true;
        if (child->type == Node::Type::Strong) hasStrong = true;
    }
    assert(hasEmphasis);
    assert(hasStrong);
    
    std::cout << "[OK] test_complex_document\n";
}

// ==================== Edge Cases ====================

void test_nested_emphasis_in_strong() {
    std::string input = "**bold and *italic***";
    Tokenizer tokenizer(input);
    Parser parser(tokenizer);
    auto doc = parser.parse_document();
    
    // Just verify it parses without crashing
    assert_node_type(doc.get(), Node::Type::Document, "test_nested_emphasis_in_strong");
    
    std::cout << "[OK] test_nested_emphasis_in_strong\n";
}

void test_unclosed_emphasis() {
    std::string input = "*unclosed";
    Tokenizer tokenizer(input);
    Parser parser(tokenizer);
    auto doc = parser.parse_document();
    
    // Should parse without crashing
    assert_node_type(doc.get(), Node::Type::Document, "test_unclosed_emphasis");
    
    std::cout << "[OK] test_unclosed_emphasis\n";
}

void test_no_error_on_valid_input() {
    std::string input = "# Hello\n\nWorld";
    Tokenizer tokenizer(input);
    Parser parser(tokenizer);
    auto doc = parser.parse_document();
    
    assert(!parser.has_error());
    
    std::cout << "[OK] test_no_error_on_valid_input\n";
}

// ==================== Main ====================

int main() {
    std::cout << "=== Parser Unit Tests ===\n\n";
    
    // Document tests
    test_empty_document();
    test_whitespace_only_document();
    
    // Heading tests
    test_simple_heading();
    test_heading_with_trailing_newline();
    
    // Paragraph tests
    test_simple_paragraph();
    test_two_paragraphs();
    
    // Emphasis tests
    test_emphasis();
    test_emphasis_in_sentence();
    
    // Strong tests
    test_strong();
    test_strong_in_sentence();
    
    // Inline code tests
    test_inline_code();
    test_inline_code_in_sentence();
    
    // Code block tests
    test_code_block_simple();
    test_code_block_with_language();
    test_code_block_multiline();
    
    // Mixed content tests
    test_heading_then_paragraph();
    test_paragraph_then_code_block();
    test_complex_document();
    
    // Edge cases
    test_nested_emphasis_in_strong();
    test_unclosed_emphasis();
    test_no_error_on_valid_input();
    
    std::cout << "\n=== All parser tests passed! ===\n";
    return 0;
}
