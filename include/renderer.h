#pragma once
#include <string>
#include "ast.h"
#include "ir.h"

std::string render_html(const Node& node);
std::string render_html(const ir::Document& doc);