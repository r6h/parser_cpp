#include <fstream>
#include <sstream>
#include <iostream>
#include <filesystem>
#include <cassert>

#include "tokenizer.h"
#include "parser.h"
#include "renderer.h"
#include "lowering.h"

std::string read_file(const std::string& path) {
    std::ifstream file(path);
    std::ostringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

void run_gold_tests(const std::string& md_path, const std::string& html_path) {
    std::string input = read_file(md_path);
    std::string expected = read_file(html_path);

    Tokenizer tokenizer(input);
    Parser parse(tokenizer);
    auto ast = parse.parse_document();
    auto ir_doc = lower_to_ir(*ast);

    std::string actual = render_html(ir_doc);

    if (actual != expected) {
        std::cerr << "[X] Test failed: " << md_path << "\n";
        std::cerr << "Expected:\n" << expected << "\n";
        std::cerr << "Actual:\n" << actual << "\n";
        std::abort(); 
    }

    std::cout << "[OK] " << md_path << "\n";
}

int main() {
    for (auto& entry : std::filesystem::directory_iterator("tests/cases")) {
        if (entry.path().extension() == ".md") {
            auto md = entry.path().string();
            auto path = entry.path();
            path.replace_extension(".html");
            auto html = path.string();
            run_gold_tests(md, html);
        }
    }
}