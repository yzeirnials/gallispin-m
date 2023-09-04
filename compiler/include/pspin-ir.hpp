#pragma once

#include <iostream>
#include <vector>
#include <unordered_map>
#include <memory>
#include "utils.hpp"
#include "pkt-layout.hpp"
#include "graph.hpp"

namespace PSPINIR {
    class HeaderRef {
    public:
        bool is_constant = false;
        bool is_meta = false;
        bool is_arg = false;
        std::string header;
        std::string field;

        HeaderRef(const std::string& hdr, const std::string& field)
            : is_meta(false),
              is_arg(false),
              is_constant(false),
              header(hdr),
              field(field) {}
            
        HeaderRef(const std::string& constant)
            : is_meta(false),
              is_arg(false),
              is_constant(true),
              header(""),
              field(constant) {}
        
        static HeaderRef Meta(const std::string& name);
        static HeaderRef Arg(const std::string& name);
        void print(std::ostream& os) const;
    };

    class PktParser {
    public:
        struct ParsingEdge{
            HeaderRef field_name;
            uint64_t value;
            ParsingEdge(std::string hdr, std::string field, uint64_t v)
                : field_name(std::move(hdr), std::move(field)),
                  value(v) {}
        }
        PacketLayout layout;
        Graph<std::string, ParsingEdge, AdjacencyList<ParsingEdge>> parse_graph;

        PktParser(
                PacketLayout l,
                Graph<std::string, ParsingEdge, AdjacencyList<ParsingEdge>> g)
            : layout(l),
              parse_graph(std::move(g)) {}
        
        void print_single_header(
                const HeaderLayout& layout,
                std::ostream& os,
                int indent = 0) const;
        void print_headerdef(std::ostream& os, int indent = 0) const;
        void print_parser(std::ostream& os, int indent = 0) const;
    }
}