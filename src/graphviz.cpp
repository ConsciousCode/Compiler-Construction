#include "graphviz.hpp"

#include <iostream>
#include <variant>

using namespace ast;

namespace {

/**
 * Utility printable for printing node names.
 */
struct N {
    size_t index;
    friend std::ostream& operator<<(std::ostream& os, N const& ps) {
        return os << "N_" << std::hex << ps.index;
    }
};

/**
 * Utility for assigning labels to a node.
 */
struct Label {
    std::string label;
    friend std::ostream& operator<<(std::ostream& os, Label const& ps) {
        return os << "[label=\"" << ps.label << "\"]";
    }
};
/**
 * Utility for printing a labeled node.
 */
struct L {
    size_t index;
    std::string label;
    friend std::ostream& operator<<(std::ostream& os, L const& ps) {
        return os << N{ps.index} << Label{ps.label};
    }
};

/**
 * Utility for printing an edge between two nodes.
 */
struct Edge {
    size_t from;
    size_t to;
    friend std::ostream& operator<<(std::ostream& os, Edge const& ps) {
        return os << N{ps.from} << " -- " << N{ps.to} << "\n";
    }
};

const char* op_str(Arith::Op op) {
    switch (op) {
    case Arith::Op::ADD:
        return "+";
    case Arith::Op::SUB:
        return "-";
    case Arith::Op::MUL:
        return "*";
    }

    return "?";
}

const char* op_str(Logical::Op op) {
    switch (op) {
    case Logical::Op::AND:
        return "and";
    case Logical::Op::OR:
        return "or";
    }

    return "?";
}

const char* op_str(Comparison::Op op) {
    switch (op) {
    case Comparison::Op::EQ:
        return "=";
    case Comparison::Op::LT:
        return "<";
    case Comparison::Op::LE:
        return "<=";
    case Comparison::Op::GT:
        return ">";
    case Comparison::Op::GE:
        return ">=";
    }

    return "?";
}

constexpr auto ind = "    ";

size_t viz_expr(const Expr& e, std::ostream& os, size_t nid);
size_t viz_cond(const Cond& c, std::ostream& os, size_t nid);
size_t viz_arith(const Arith& a, std::ostream& os, size_t nid);
size_t viz_block(const Block& b, std::ostream& os, size_t nid);
size_t viz_cmd(const Cmd& c, std::ostream& os, size_t nid);
static size_t viz_not(const Not& n, std::ostream& os, size_t nid);

size_t viz_arith(const Arith& a, std::ostream& os, size_t nid) {
    size_t root = nid;
    os << ind << L{root, op_str(a.op)} << "\n";

    os << ind << Edge{root, ++nid};
    nid = viz_expr(a.left, os, nid);

    os << ind << Edge{root, ++nid};
    nid = viz_expr(a.right, os, nid);

    return nid;
}

size_t viz_expr(const Expr& e, std::ostream& os, size_t nid) {
    return std::visit(
        [&](const auto& node) {
            using T = std::decay_t<decltype(node)>;
            if constexpr (std::is_same_v<T, int>) {
                os << ind << N{nid} << "[label=\"" << node << "\"]\n";
                return nid;
            } else if constexpr (std::is_same_v<T, std::string>) {
                os << ind << N{nid} << "[label=\"" << node << "\"]\n";
                return nid;
            } else {
                return viz_arith(*node, os, nid);
            }
        },
        e);
}

size_t viz_logical(const Logical& l, std::ostream& os, size_t nid) {
    size_t parent_nid = nid;

    os << ind << L{parent_nid, std::string(op_str(l.op)) + "\\n" + std::to_string(l.get_label())}
       << "\n";

    size_t left_nid = nid + 1;
    os << ind << Edge{parent_nid, left_nid};
    nid = viz_cond(l.left, os, left_nid);

    size_t right_nid = nid + 1;
    os << ind << Edge{parent_nid, right_nid};
    nid = viz_cond(l.right, os, right_nid);

    return nid;
}

size_t viz_not(const Not& n, std::ostream& os, size_t nid) {
    os << ind << L{nid, std::string("not\\n") + std::to_string(n.get_label())} << "\n";
    os << ind << Edge{nid, ++nid};
    nid = viz_cond(n.cond, os, nid);

    return nid;
}

size_t viz_cmp(const Comparison& c, std::ostream& os, size_t nid) {
    size_t root = nid;
    os << ind << L{root, std::string(op_str(c.op)) + "\\n" + std::to_string(c.get_label())} << "\n";

    os << ind << Edge{root, ++nid};
    nid = viz_expr(c.left, os, nid);

    os << ind << Edge{root, ++nid};
    nid = viz_expr(c.right, os, nid);

    return nid;
}

size_t viz_cond(const Cond& c, std::ostream& os, size_t nid) {
    return std::visit(
        [&](const auto& node) {
            using T = std::decay_t<decltype(node)>;
            if constexpr (std::is_same_v<T, std::shared_ptr<BoolConst>>) {
                os << ind
                   << L{nid, std::string(node->value ? "true" : "false") + "\\n" +
                                 std::to_string(node->get_label())}
                   << "\n";
                return nid;
            } else if constexpr (std::is_same_v<T, std::shared_ptr<Not>>) {
                return viz_not(*node, os, nid);
            } else if constexpr (std::is_same_v<T, std::shared_ptr<Logical>>) {
                return viz_logical(*node, os, nid);
            } else if constexpr (std::is_same_v<T, std::shared_ptr<BoolConst>>) {
                os << ind << L{nid, node->value ? "true" : "false"} << "\n";
                return nid;
            } else {
                return viz_cmp(*node, os, nid);
            }
        },
        c);
}

size_t viz_if(const If& s, std::ostream& os, size_t nid) {
    size_t root = nid;
    os << ind << L{nid, "if"} << "\n";

    os << ind << Edge{root, ++nid};
    nid = viz_cond(s.cond, os, nid);

    os << ind << Edge{root, ++nid};
    nid = viz_block(s.then, os, nid);

    os << ind << Edge{root, ++nid};
    nid = viz_block(s.el, os, nid);

    return nid;
}

size_t viz_while(const While& w, std::ostream& os, size_t nid) {
    size_t root = nid;
    os << ind << L{root, "while"} << "\n";

    os << ind << Edge{root, ++nid};
    nid = viz_cond(w.cond, os, nid);

    os << ind << Edge{root, ++nid};
    nid = viz_block(w.body, os, nid);

    return nid;
}

size_t viz_assign(const Assign& a, std::ostream& os, size_t nid) {
    size_t root = nid;
    os << ind << L{root, std::string(":=\\n") + std::to_string(a.get_label())} << "\n";

    os << ind << Edge{root, ++nid};
    os << ind << L{nid, a.name} << "\n";

    os << ind << Edge{root, ++nid};
    nid = viz_expr(a.value, os, nid);

    return nid;
}

size_t viz_cmd(const Cmd& c, std::ostream& os, size_t nid) {
    return std::visit(
        [&](const auto& node) {
            using T = std::decay_t<decltype(node)>;
            if constexpr (std::is_same_v<T, Assign>) {
                return viz_assign(node, os, nid);
            } else if constexpr (std::is_same_v<T, If>) {
                return viz_if(node, os, nid);
            } else if constexpr (std::is_same_v<T, While>) {
                return viz_while(node, os, nid);
            } else {
                static_assert(std::is_same_v<T, Skip>);
                os << ind << L{nid, std::string("skip\\n") + std::to_string(node.get_label())}
                   << "\n";
                return nid;
            }
        },
        c);
}

size_t viz_block(const Block& b, std::ostream& os, size_t nid) {
    size_t root = nid;
    os << ind << L{root, "block"} << "\n";
    for (size_t i = 0; i < b.cmds.size(); i++) {
        os << ind << Edge{root, ++nid};
        nid = viz_cmd(b.cmds[i], os, nid);
    }

    return nid;
}
} // namespace

namespace graphviz {
void print_viz_block(const Block& blk) {
    std::cout << "strict graph {\n";
    viz_block(blk, std::cout, 0);
    std::cout << "}\n";
}
} // namespace graphviz
