#pragma once

#include "parser.hpp"

using namespace whilelang;
using namespace parser;

using P = parser::Production;

parser::ParserGenerator generate_while_parser();
