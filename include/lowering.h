#pragma once
#include "ast.h"
#include "ir.h"

ir::Document lower_to_ir(const Node& node);
