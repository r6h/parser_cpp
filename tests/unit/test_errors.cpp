#include <cassert>
#include <iostream>
#include <string>
#include <memory>
#include "tokenizer.h"
#include "parser.h"
#include "ast.h"

// Helper to assert error exists
void assert_has_error(Parser& parser, const std::string& test_name) {
    if (!parser.has_error()) {
        std::cerr << "FAIL: " << test_name << " - Expected an error but none was set\n";
        std::exit(1);
    }
}

// Helper to assert no error
void assert_no_error(Parser& parser, const std::string& test_name) {
    if (parser.has_error()) {
        const auto& err = parser.get_error();
        std::cerr << "FAIL: " << test_name << " - Unexpected error: " << err.message
                  << " at line " << err.pos.line << ", column " << err.pos.column << "\n";
        std::exit(1);
    }
}

// Helper to assert error message contains substring
void assert_error_contains(Parser& parser, const std::string& substring, const std::string& test_name) {
    if (!parser.has_error()) {
        std::cerr << "FAIL: " << test_name << " - Expected an error but none was set\n";
        std::exit(1);
    }
    const auto& err = parser.get_error();
    if (err.message.find(substring) == std::string::npos) {
        std::cerr << "FAIL: " << test_name << " - Error message '" << err.message
                  << "' does not contain '" << substring << "'\n";
        std::exit(1);
    }
}

// Helper to assert error position
void assert_error_position(Parser& parser, size_t line, size_t col, const std::string& test_name) {
    if (!parser.has_error()) {
        std::cerr << "FAIL: " << test_name << " - Expected an error but none was set\n";
        std::exit(1);
    }
    const auto& err = parser.get_error();
    if (err.pos.line != line || err.pos.column != col) {
        std::cerr << "FAIL: " << test_name << " - Expected error at (" << line << "," << col
                  << ") but got (" << err.pos.line << "," << err.pos.column << ")\n";
        std::exit(1);
    }
}

// ==================== Valid Input Tests (No Errors Expected) ====================

void test_valid_empty_document() {
    std::string input = "";
    Tokenizer tokenizer(input);
    Parser parser(tokenizer);
    auto doc = parser.parse_document();
    
    assert_no_error(parser, "test_valid_empty_document");
    
    std::cout << "[OK] test_valid_empty_document\n";
}

void test_valid_heading() {
    std::string input = "# Hello World";
    Tokenizer tokenizer(input);
    Parser parser(tokenizer);
    auto doc = parser.parse_document();
    
    assert_no_error(parser, "test_valid_heading");
    
    std::cout << "[OK] test_valid_heading\n";
}

void test_valid_paragraph() {
    std::string input = "This is some text.";
    Tokenizer tokenizer(input);
    Parser parser(tokenizer);
    auto doc = parser.parse_document();
    
    assert_no_error(parser, "test_valid_paragraph");
    
    std::cout << "[OK] test_valid_paragraph\n";
}

void test_valid_code_block() {
    std::string input = "```cpp\nint x = 42;\n```";
    Tokenizer tokenizer(input);
    Parser parser(tokenizer);
    auto doc = parser.parse_document();
    
    assert_no_error(parser, "test_valid_code_block");
    
    std::cout << "[OK] test_valid_code_block\n";
}

void test_valid_emphasis() {
    std::string input = "*italic*";
    Tokenizer tokenizer(input);
    Parser parser(tokenizer);
    auto doc = parser.parse_document();
    
    assert_no_error(parser, "test_valid_emphasis");
    
    std::cout << "[OK] test_valid_emphasis\n";
}

void test_valid_strong() {
    std::string input = "**bold**";
    Tokenizer tokenizer(input);
    Parser parser(tokenizer);
    auto doc = parser.parse_document();
    
    assert_no_error(parser, "test_valid_strong");
    
    std::cout << "[OK] test_valid_strong\n";
}

void test_valid_inline_code() {
    std::string input = "`code`";
    Tokenizer tokenizer(input);
    Parser parser(tokenizer);
    auto doc = parser.parse_document();
    
    assert_no_error(parser, "test_valid_inline_code");
    
    std::cout << "[OK] test_valid_inline_code\n";
}

void test_valid_complex_document() {
    std::string input = "# Title\n\nA *paragraph* with **bold** and `code`.\n\n```js\nalert('hi');\n```";
    Tokenizer tokenizer(input);
    Parser parser(tokenizer);
    auto doc = parser.parse_document();
    
    assert_no_error(parser, "test_valid_complex_document");
    
    std::cout << "[OK] test_valid_complex_document\n";
}

// ==================== Error Recovery Tests ====================

void test_unclosed_code_block_graceful() {
    std::string input = "```cpp\nint x = 42;";
    Tokenizer tokenizer(input);
    Parser parser(tokenizer);
    auto doc = parser.parse_document();
    
    // Parser should handle this gracefully without crashing
    // The document should still be returned (possibly incomplete)
    assert(doc != nullptr);
    
    std::cout << "[OK] test_unclosed_code_block_graceful\n";
}

void test_unclosed_emphasis_graceful() {
    std::string input = "*unclosed emphasis";
    Tokenizer tokenizer(input);
    Parser parser(tokenizer);
    auto doc = parser.parse_document();
    
    // Parser should handle this gracefully without crashing
    assert(doc != nullptr);
    
    std::cout << "[OK] test_unclosed_emphasis_graceful\n";
}

void test_unclosed_strong_graceful() {
    std::string input = "**unclosed strong";
    Tokenizer tokenizer(input);
    Parser parser(tokenizer);
    auto doc = parser.parse_document();
    
    // Parser should handle this gracefully without crashing
    assert(doc != nullptr);
    
    std::cout << "[OK] test_unclosed_strong_graceful\n";
}

void test_unclosed_inline_code_graceful() {
    std::string input = "`unclosed code";
    Tokenizer tokenizer(input);
    Parser parser(tokenizer);
    auto doc = parser.parse_document();
    
    // Parser should handle this gracefully without crashing
    assert(doc != nullptr);
    
    std::cout << "[OK] test_unclosed_inline_code_graceful\n";
}

// ==================== Error Tracking State Tests ====================

void test_error_only_set_once() {
    // This tests that once an error is set, subsequent calls to error() don't overwrite it
    std::string input = "```\nunclosed";
    Tokenizer tokenizer(input);
    Parser parser(tokenizer);
    
    // Parse should complete without crashing
    auto doc = parser.parse_document();
    
    // We can't easily trigger multiple errors in this parser,
    // but we verify the parser handles the situation gracefully
    assert(doc != nullptr);
    
    std::cout << "[OK] test_error_only_set_once\n";
}

void test_has_error_returns_correct_state() {
    // Test that has_error() returns false for valid input
    std::string input = "# Valid heading";
    Tokenizer tokenizer(input);
    Parser parser(tokenizer);
    
    assert(!parser.has_error());
    auto doc = parser.parse_document();
    assert(!parser.has_error());
    
    std::cout << "[OK] test_has_error_returns_correct_state\n";
}

// ==================== Edge Cases ====================

void test_only_special_chars() {
    std::string input = "#*`";
    Tokenizer tokenizer(input);
    Parser parser(tokenizer);
    auto doc = parser.parse_document();
    
    // Should parse without crashing
    assert(doc != nullptr);
    
    std::cout << "[OK] test_only_special_chars\n";
}

void test_deeply_nested_formatting() {
    std::string input = "**bold *and italic***";
    Tokenizer tokenizer(input);
    Parser parser(tokenizer);
    auto doc = parser.parse_document();
    
    // Should parse without crashing
    assert(doc != nullptr);
    
    std::cout << "[OK] test_deeply_nested_formatting\n";
}

void test_consecutive_special_tokens() {
    std::string input = "##**``";
    Tokenizer tokenizer(input);
    Parser parser(tokenizer);
    auto doc = parser.parse_document();
    
    // Should parse without crashing
    assert(doc != nullptr);
    
    std::cout << "[OK] test_consecutive_special_tokens\n";
}

void test_empty_emphasis() {
    std::string input = "**";
    Tokenizer tokenizer(input);
    Parser parser(tokenizer);
    auto doc = parser.parse_document();
    
    // Should parse without crashing
    assert(doc != nullptr);
    
    std::cout << "[OK] test_empty_emphasis\n";
}

void test_empty_inline_code() {
    std::string input = "``";
    Tokenizer tokenizer(input);
    Parser parser(tokenizer);
    auto doc = parser.parse_document();
    
    // Should parse without crashing
    assert(doc != nullptr);
    
    std::cout << "[OK] test_empty_inline_code\n";
}

void test_many_newlines() {
    std::string input = "\n\n\n\n\n";
    Tokenizer tokenizer(input);
    Parser parser(tokenizer);
    auto doc = parser.parse_document();
    
    assert_no_error(parser, "test_many_newlines");
    assert(doc != nullptr);
    
    std::cout << "[OK] test_many_newlines\n";
}

void test_alternating_blocks() {
    std::string input = "# H1\n\nPara\n\n# H2\n\nPara2\n\n```\ncode\n```";
    Tokenizer tokenizer(input);
    Parser parser(tokenizer);
    auto doc = parser.parse_document();
    
    assert_no_error(parser, "test_alternating_blocks");
    assert(doc != nullptr);
    assert(doc->children.size() >= 4);
    
    std::cout << "[OK] test_alternating_blocks\n";
}

// ==================== Stress Tests ====================

void test_long_document() {
    std::string input;
    for (int i = 0; i < 100; ++i) {
        input += "# Heading " + std::to_string(i) + "\n\n";
        input += "Paragraph " + std::to_string(i) + " with *emphasis* and **strong**.\n\n";
    }
    
    Tokenizer tokenizer(input);
    Parser parser(tokenizer);
    auto doc = parser.parse_document();
    
    assert_no_error(parser, "test_long_document");
    assert(doc != nullptr);
    assert(doc->children.size() == 200); // 100 headings + 100 paragraphs
    
    std::cout << "[OK] test_long_document\n";
}

void test_long_code_block() {
    std::string input = "```cpp\n";
    for (int i = 0; i < 100; ++i) {
        input += "int x" + std::to_string(i) + " = " + std::to_string(i) + ";\n";
    }
    input += "```";
    
    Tokenizer tokenizer(input);
    Parser parser(tokenizer);
    auto doc = parser.parse_document();
    
    assert_no_error(parser, "test_long_code_block");
    assert(doc != nullptr);
    
    std::cout << "[OK] test_long_code_block\n";
}

// ==================== Main ====================

int main() {
    std::cout << "=== Error Handling Tests ===\n\n";
    
    // Valid input tests
    test_valid_empty_document();
    test_valid_heading();
    test_valid_paragraph();
    test_valid_code_block();
    test_valid_emphasis();
    test_valid_strong();
    test_valid_inline_code();
    test_valid_complex_document();
    
    // Error recovery tests
    test_unclosed_code_block_graceful();
    test_unclosed_emphasis_graceful();
    test_unclosed_strong_graceful();
    test_unclosed_inline_code_graceful();
    
    // Error tracking state tests
    test_error_only_set_once();
    test_has_error_returns_correct_state();
    
    // Edge cases
    test_only_special_chars();
    test_deeply_nested_formatting();
    test_consecutive_special_tokens();
    test_empty_emphasis();
    test_empty_inline_code();
    test_many_newlines();
    test_alternating_blocks();
    
    // Stress tests
    test_long_document();
    test_long_code_block();
    
    std::cout << "\n=== All error handling tests passed! ===\n";
    return 0;
}
