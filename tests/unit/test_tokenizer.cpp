#include <cassert>
#include <iostream>
#include <string>
#include "tokenizer.h"

// Helper to check token type
void assert_token_type(const Token& tok, TokenType expected, const std::string& test_name) {
    if (tok.type != expected) {
        std::cerr << "FAIL: " << test_name << " - Expected token type mismatch\n";
        std::exit(1);
    }
}

// Helper to check position
void assert_position(const Token& tok, size_t line, size_t col, const std::string& test_name) {
    if (tok.position.line != line || tok.position.column != col) {
        std::cerr << "FAIL: " << test_name << " - Expected position ("
                  << line << "," << col << ") but got ("
                  << tok.position.line << "," << tok.position.column << ")\n";
        std::exit(1);
    }
}

// ==================== Basic Token Tests ====================

void test_empty_input() {
    std::string input = "";
    Tokenizer t(input);
    Token tok = t.next();
    assert_token_type(tok, TokenType::EndOfFile, "test_empty_input");
    std::cout << "[OK] test_empty_input\n";
}

void test_hash_token() {
    std::string input = "#";
    Tokenizer t(input);
    Token tok = t.next();
    assert_token_type(tok, TokenType::Hash, "test_hash_token");
    assert_position(tok, 1, 1, "test_hash_token");
    std::cout << "[OK] test_hash_token\n";
}

void test_star_token() {
    std::string input = "*";
    Tokenizer t(input);
    Token tok = t.next();
    assert_token_type(tok, TokenType::Star, "test_star_token");
    assert_position(tok, 1, 1, "test_star_token");
    std::cout << "[OK] test_star_token\n";
}

void test_backtick_token() {
    std::string input = "`";
    Tokenizer t(input);
    Token tok = t.next();
    assert_token_type(tok, TokenType::Backtick, "test_backtick_token");
    assert_position(tok, 1, 1, "test_backtick_token");
    std::cout << "[OK] test_backtick_token\n";
}

void test_newline_token() {
    std::string input = "\n";
    Tokenizer t(input);
    Token tok = t.next();
    assert_token_type(tok, TokenType::Newline, "test_newline_token");
    assert_position(tok, 1, 1, "test_newline_token");
    std::cout << "[OK] test_newline_token\n";
}

void test_fence_token() {
    std::string input = "```";
    Tokenizer t(input);
    Token tok = t.next();
    assert_token_type(tok, TokenType::Fence, "test_fence_token");
    assert_position(tok, 1, 1, "test_fence_token");
    assert(tok.lexeme == "```");
    std::cout << "[OK] test_fence_token\n";
}

void test_text_token() {
    std::string input = "hello";
    Tokenizer t(input);
    Token tok = t.next();
    assert_token_type(tok, TokenType::Text, "test_text_token");
    assert(tok.lexeme == "hello");
    assert_position(tok, 1, 1, "test_text_token");
    std::cout << "[OK] test_text_token\n";
}

// ==================== Line/Column Tracking Tests ====================

void test_line_tracking_single_newline() {
    std::string input = "a\nb";
    Tokenizer t(input);
    
    Token t1 = t.next(); // 'a'
    assert_token_type(t1, TokenType::Text, "test_line_tracking t1");
    assert_position(t1, 1, 1, "test_line_tracking t1");
    
    Token t2 = t.next(); // '\n'
    assert_token_type(t2, TokenType::Newline, "test_line_tracking t2");
    assert_position(t2, 1, 2, "test_line_tracking t2");
    
    Token t3 = t.next(); // 'b'
    assert_token_type(t3, TokenType::Text, "test_line_tracking t3");
    assert_position(t3, 2, 1, "test_line_tracking t3");
    
    std::cout << "[OK] test_line_tracking_single_newline\n";
}

void test_line_tracking_multiple_newlines() {
    std::string input = "a\n\nb";
    Tokenizer t(input);
    
    Token t1 = t.next(); // 'a'
    assert_position(t1, 1, 1, "test_line_tracking_multiple t1");
    
    Token t2 = t.next(); // '\n'
    assert_position(t2, 1, 2, "test_line_tracking_multiple t2");
    
    Token t3 = t.next(); // '\n'
    assert_position(t3, 2, 1, "test_line_tracking_multiple t3");
    
    Token t4 = t.next(); // 'b'
    assert_position(t4, 3, 1, "test_line_tracking_multiple t4");
    
    std::cout << "[OK] test_line_tracking_multiple_newlines\n";
}

void test_column_tracking_text() {
    std::string input = "abc#";
    Tokenizer t(input);
    
    Token t1 = t.next(); // 'abc'
    assert_token_type(t1, TokenType::Text, "test_column_tracking t1");
    assert_position(t1, 1, 1, "test_column_tracking t1");
    assert(t1.lexeme == "abc");
    
    Token t2 = t.next(); // '#'
    assert_token_type(t2, TokenType::Hash, "test_column_tracking t2");
    assert_position(t2, 1, 4, "test_column_tracking t2");
    
    std::cout << "[OK] test_column_tracking_text\n";
}

void test_column_tracking_fence() {
    std::string input = "x```y";
    Tokenizer t(input);
    
    Token t1 = t.next(); // 'x'
    assert_position(t1, 1, 1, "test_column_tracking_fence t1");
    
    Token t2 = t.next(); // '```'
    assert_token_type(t2, TokenType::Fence, "test_column_tracking_fence t2");
    assert_position(t2, 1, 2, "test_column_tracking_fence t2");
    
    Token t3 = t.next(); // 'y'
    assert_position(t3, 1, 5, "test_column_tracking_fence t3");
    
    std::cout << "[OK] test_column_tracking_fence\n";
}

// ==================== Sequence Tests ====================

void test_heading_sequence() {
    std::string input = "# Hello";
    Tokenizer t(input);
    
    Token t1 = t.next();
    assert_token_type(t1, TokenType::Hash, "test_heading_sequence t1");
    
    Token t2 = t.next();
    assert_token_type(t2, TokenType::Text, "test_heading_sequence t2");
    assert(t2.lexeme == " Hello");
    
    Token t3 = t.next();
    assert_token_type(t3, TokenType::EndOfFile, "test_heading_sequence t3");
    
    std::cout << "[OK] test_heading_sequence\n";
}

void test_emphasis_sequence() {
    std::string input = "*hello*";
    Tokenizer t(input);
    
    Token t1 = t.next();
    assert_token_type(t1, TokenType::Star, "test_emphasis_sequence t1");
    
    Token t2 = t.next();
    assert_token_type(t2, TokenType::Text, "test_emphasis_sequence t2");
    assert(t2.lexeme == "hello");
    
    Token t3 = t.next();
    assert_token_type(t3, TokenType::Star, "test_emphasis_sequence t3");
    
    std::cout << "[OK] test_emphasis_sequence\n";
}

void test_strong_sequence() {
    std::string input = "**bold**";
    Tokenizer t(input);
    
    Token t1 = t.next();
    assert_token_type(t1, TokenType::Star, "test_strong_sequence t1");
    
    Token t2 = t.next();
    assert_token_type(t2, TokenType::Star, "test_strong_sequence t2");
    
    Token t3 = t.next();
    assert_token_type(t3, TokenType::Text, "test_strong_sequence t3");
    assert(t3.lexeme == "bold");
    
    Token t4 = t.next();
    assert_token_type(t4, TokenType::Star, "test_strong_sequence t4");
    
    Token t5 = t.next();
    assert_token_type(t5, TokenType::Star, "test_strong_sequence t5");
    
    std::cout << "[OK] test_strong_sequence\n";
}

void test_inline_code_sequence() {
    std::string input = "`code`";
    Tokenizer t(input);
    
    Token t1 = t.next();
    assert_token_type(t1, TokenType::Backtick, "test_inline_code_sequence t1");
    
    Token t2 = t.next();
    assert_token_type(t2, TokenType::Text, "test_inline_code_sequence t2");
    assert(t2.lexeme == "code");
    
    Token t3 = t.next();
    assert_token_type(t3, TokenType::Backtick, "test_inline_code_sequence t3");
    
    std::cout << "[OK] test_inline_code_sequence\n";
}

void test_code_block_sequence() {
    std::string input = "```cpp\nint x;\n```";
    Tokenizer t(input);
    
    Token t1 = t.next();
    assert_token_type(t1, TokenType::Fence, "test_code_block_sequence t1");
    
    Token t2 = t.next();
    assert_token_type(t2, TokenType::Text, "test_code_block_sequence t2");
    assert(t2.lexeme == "cpp");
    
    Token t3 = t.next();
    assert_token_type(t3, TokenType::Newline, "test_code_block_sequence t3");
    
    Token t4 = t.next();
    assert_token_type(t4, TokenType::Text, "test_code_block_sequence t4");
    assert(t4.lexeme == "int x;");
    
    Token t5 = t.next();
    assert_token_type(t5, TokenType::Newline, "test_code_block_sequence t5");
    
    Token t6 = t.next();
    assert_token_type(t6, TokenType::Fence, "test_code_block_sequence t6");
    
    std::cout << "[OK] test_code_block_sequence\n";
}

// ==================== Peek Tests ====================

void test_peek_does_not_advance() {
    std::string input = "#*";
    Tokenizer t(input);
    
    Token peeked = t.peek(1);
    assert_token_type(peeked, TokenType::Hash, "test_peek_does_not_advance peek");
    
    Token actual = t.next();
    assert_token_type(actual, TokenType::Hash, "test_peek_does_not_advance next");
    
    std::cout << "[OK] test_peek_does_not_advance\n";
}

void test_peek_multiple() {
    std::string input = "#*`";
    Tokenizer t(input);
    
    Token p1 = t.peek(1);
    assert_token_type(p1, TokenType::Hash, "test_peek_multiple p1");
    
    Token p2 = t.peek(2);
    assert_token_type(p2, TokenType::Star, "test_peek_multiple p2");
    
    Token p3 = t.peek(3);
    assert_token_type(p3, TokenType::Backtick, "test_peek_multiple p3");
    
    // Position should still be at start
    Token actual = t.next();
    assert_token_type(actual, TokenType::Hash, "test_peek_multiple next");
    
    std::cout << "[OK] test_peek_multiple\n";
}

void test_peek_preserves_line_column() {
    std::string input = "a\nb";
    Tokenizer t(input);
    
    // Peek ahead to 'b' (3 tokens: 'a', '\n', 'b')
    Token peeked = t.peek(3);
    assert_token_type(peeked, TokenType::Text, "test_peek_preserves_line_column peek");
    assert_position(peeked, 2, 1, "test_peek_preserves_line_column peek pos");
    
    // Now get the first token - should still be at line 1, col 1
    Token first = t.next();
    assert_position(first, 1, 1, "test_peek_preserves_line_column first");
    
    std::cout << "[OK] test_peek_preserves_line_column\n";
}

// ==================== Edge Cases ====================

void test_only_newlines() {
    std::string input = "\n\n\n";
    Tokenizer t(input);
    
    Token t1 = t.next();
    assert_token_type(t1, TokenType::Newline, "test_only_newlines t1");
    assert_position(t1, 1, 1, "test_only_newlines t1");
    
    Token t2 = t.next();
    assert_token_type(t2, TokenType::Newline, "test_only_newlines t2");
    assert_position(t2, 2, 1, "test_only_newlines t2");
    
    Token t3 = t.next();
    assert_token_type(t3, TokenType::Newline, "test_only_newlines t3");
    assert_position(t3, 3, 1, "test_only_newlines t3");
    
    Token t4 = t.next();
    assert_token_type(t4, TokenType::EndOfFile, "test_only_newlines t4");
    
    std::cout << "[OK] test_only_newlines\n";
}

void test_two_backticks_not_fence() {
    std::string input = "``";
    Tokenizer t(input);
    
    Token t1 = t.next();
    assert_token_type(t1, TokenType::Backtick, "test_two_backticks_not_fence t1");
    
    Token t2 = t.next();
    assert_token_type(t2, TokenType::Backtick, "test_two_backticks_not_fence t2");
    
    std::cout << "[OK] test_two_backticks_not_fence\n";
}

void test_four_backticks() {
    std::string input = "````";
    Tokenizer t(input);
    
    Token t1 = t.next();
    assert_token_type(t1, TokenType::Fence, "test_four_backticks t1");
    
    Token t2 = t.next();
    assert_token_type(t2, TokenType::Backtick, "test_four_backticks t2");
    
    std::cout << "[OK] test_four_backticks\n";
}

void test_mixed_special_chars() {
    std::string input = "#*`\n";
    Tokenizer t(input);
    
    Token t1 = t.next();
    assert_token_type(t1, TokenType::Hash, "test_mixed_special_chars t1");
    
    Token t2 = t.next();
    assert_token_type(t2, TokenType::Star, "test_mixed_special_chars t2");
    
    Token t3 = t.next();
    assert_token_type(t3, TokenType::Backtick, "test_mixed_special_chars t3");
    
    Token t4 = t.next();
    assert_token_type(t4, TokenType::Newline, "test_mixed_special_chars t4");
    
    std::cout << "[OK] test_mixed_special_chars\n";
}

void test_long_text() {
    std::string input = "This is a longer piece of text without special characters";
    Tokenizer t(input);
    
    Token tok = t.next();
    assert_token_type(tok, TokenType::Text, "test_long_text");
    assert(tok.lexeme == input);
    
    std::cout << "[OK] test_long_text\n";
}

// ==================== Main ====================

int main() {
    std::cout << "=== Tokenizer Unit Tests ===\n\n";
    
    // Basic token tests
    test_empty_input();
    test_hash_token();
    test_star_token();
    test_backtick_token();
    test_newline_token();
    test_fence_token();
    test_text_token();
    
    // Line/column tracking tests
    test_line_tracking_single_newline();
    test_line_tracking_multiple_newlines();
    test_column_tracking_text();
    test_column_tracking_fence();
    
    // Sequence tests
    test_heading_sequence();
    test_emphasis_sequence();
    test_strong_sequence();
    test_inline_code_sequence();
    test_code_block_sequence();
    
    // Peek tests
    test_peek_does_not_advance();
    test_peek_multiple();
    test_peek_preserves_line_column();
    
    // Edge cases
    test_only_newlines();
    test_two_backticks_not_fence();
    test_four_backticks();
    test_mixed_special_chars();
    test_long_text();
    
    std::cout << "\n=== All tokenizer tests passed! ===\n";
    return 0;
}
