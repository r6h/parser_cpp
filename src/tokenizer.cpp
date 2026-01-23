#include "tokenizer.h"
#include "token.h"
#include <cstddef>
#include <string_view>

Token Tokenizer::next() {

  if (pos_ >= input_.size()) {
    return Token{TokenType::EndOfFile, "", pos_};
  }

  size_t start = pos_;
  char c = input_[pos_];

  // Control characters
  if (c == '#') {
    pos_++;
    return Token{TokenType::Hash, {}, start};
  }
  if (c == '*') {
    pos_++;
    return Token{TokenType::Star, {}, start};
  }
  if (c == '`') {
    pos_++;
    return Token{TokenType::Backtick, {}, start};
  }
  if (c == '\n') {
    pos_++;
    return Token{TokenType::Newline, {}, start};
  }

  // TEXT token: consume until control character or EOF
  while (pos_ < input_.size()) {
    char cc = input_[pos_];
    if (cc == '#' || cc == '*' || cc == '`' || cc == '\n')
      break;
    ++pos_;
  }
  return Token{TokenType::Text, std::string_view(&input_[start], pos_ - start),
               start};
}

Token Tokenizer::peek(size_t n) {
  size_t saved_pos = pos_;

  Token t{TokenType::EndOfFile, {}, pos_};
  for (size_t i = 0; i < n && pos_ < input_.size(); ++i) {
    t = next();
  }

  pos_ = saved_pos;
  return t;
}
