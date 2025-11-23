#pragma once

#include "cfg.hpp"

#include <iosfwd>

namespace cfg {
    void print_cfg_dot(const CFG& g, std::ostream& out, bool show_vars);
}