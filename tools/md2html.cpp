#include <fstream>
#include <iostream>
#include <sstream>

#include "tokenizer.h"
#include "parser.h"
#include "renderer.h"
#include "lowering.h"

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "Usage: md2html <input.md>\n";
        return 1;
    } 

    std::ifstream file(argv[1]);
    if (!file) {
        std::cerr << "Could not open file: " << argv[1] << "\n";
        return 1;
    }

    std::ostringstream buffer;
    buffer << file.rdbuf();
    std::string input = buffer.str();

    std::cerr << "DEBUG: Read " << input.length() << " bytes\n";
    std::cerr << "DEBUG: File contents:\n[" << input << "]\n";

    Tokenizer tokenizer(input);
    Parser parser(tokenizer);
    auto ast = parser.parse_document();
    auto ir_doc = lower_to_ir(*ast);

    std::cout << render_html(ir_doc);
    return 0;
}