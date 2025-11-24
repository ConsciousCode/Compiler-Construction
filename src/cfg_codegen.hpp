#include "cfg.hpp"

namespace cfg {

/**
 * Generate RISC-V pseudocode from a CFG using infinite sN registers.
 **/
std::string cfg_codegen(const cfg::CFG& cfg, const std::string& program_name);

} // namespace cfg