#pragma once

#include "ast.hpp"
#include "cfg.hpp"

namespace cfg {
CFG build_cfg(const ast::Block& program);
}