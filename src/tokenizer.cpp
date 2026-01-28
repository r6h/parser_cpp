#include "tokenizer.h"
#include <cassert>
#include "token.h"
#include <cstddef>
#include <string_view>

// helper to detect start of code block fence (```)
bool Tokenizer::is_fence_start(size_t pos) const {
  return pos + 2 < input_.size() &&
       input_[pos] == '`' &&
       input_[pos+1] == '`' &&
       input_[pos+2] == '`';
}

void Tokenizer::advance_pos() {
  if (pos_ < input_.size()) {
    if (input_[pos_] == '\n') {
      line_++;
      column_ = 1;
    } else {
      column_++;
    }
    pos_++;
  }
}

Token Tokenizer::next() {

  if (pos_ >= input_.size()) {
    return Token{TokenType::EndOfFile, "", SourcePos{line_, column_}};
  }

  size_t start = pos_;
  size_t start_line = line_;
  size_t start_column = column_;
  char c = input_[pos_];

  // Control characters
  if (c == '#') {
    advance_pos();
    return Token{TokenType::Hash, {}, SourcePos{start_line, start_column}};
  }
  if (c == '*') {
    advance_pos();
    return Token{TokenType::Star, {}, SourcePos{start_line, start_column}};
  }
  if (c == '`') {
    // check for fence (```)
    if (is_fence_start(pos_)) {
      size_t start = pos_;
      advance_pos();
      advance_pos();
      advance_pos();
      return Token{TokenType::Fence, std::string_view(&input_[start], 3), SourcePos{start_line, start_column}};
    }
    advance_pos();
    return Token{TokenType::Backtick, {}, SourcePos{start_line, start_column}};
  }
  if (c == '\n') {
    advance_pos();
    return Token{TokenType::Newline, {}, SourcePos{start_line, start_column}};
  }

  // TEXT token: consume until control character or EOF
  while (pos_ < input_.size()) {
    char cc = input_[pos_];
    if (cc == '#' || cc == '*' || cc == '`' || cc == '\n')
      break;
    advance_pos();
  }
  return Token{TokenType::Text, std::string_view(&input_[start], pos_ - start),
               SourcePos{start_line, start_column}};
}

Token Tokenizer::peek(size_t n) {
  size_t saved_pos = pos_;
  size_t saved_line = line_;
  size_t saved_column = column_;

  Token t{TokenType::EndOfFile, {}, SourcePos{line_, column_}};
  for (size_t i = 0; i < n && pos_ < input_.size(); ++i) {
    t = next();
  }

  pos_ = saved_pos;
  line_ = saved_line;
  column_ = saved_column;

  // invariant check
  assert(pos_ == saved_pos);
  
  return t;
}
