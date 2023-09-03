#include "hilvl-ir.hpp"
#include "hir-common-pass.hpp"
#include "llvm-load.hpp"
#include "hir-pktop.hpp"
#include "hir-stateop.hpp"
// above is for generating HIR
#include "hir-partition.hpp"
#include "hir-p4.hpp"
// above is for Labeling
#include <fstream>
// above is for Partition
#include <iostream>


int main(int argc, char *argv[]) {
    LLVMStore store;
    store.load_directory("../../click-llvm-ir/ele_ll/tcpudp");
    store.load_directory("../../click-llvm-ir/lib_ll");

    auto m = std::make_shared<HIR::Module>();
    auto ele = std::make_shared<HIR::Element>(*m, store, "MyIPRewriter");
    element_function_inline(*ele);
    replace_packet_access_op(*ele, CommonHdr::default_layout);
    remove_unused_phi_entry(*ele->entry());

    split_stateptr_branch(*ele);
    replace_vector_ops(*ele);
    replace_map_ops(*ele);
    replace_regular_struct_access(*ele->entry());
    replace_packet_meta_op(*ele);
    remove_unused_ops(*ele);
    
    // ele->print(std::cout);
    // above is generating HIR

    HIR::LabelInitFn init_fn = P4IR::p4_initial_label;
    HIR::label(*ele, init_fn);
    // P4IR::print_element_with_label(std::cout, *ele);
    // above is for Labeling

    auto partition_result = partition(*ele->entry());

    std::ofstream file;
    file.open("part-PRE.hir", std::ios::trunc);
    auto printer = [](std::ostream& os, const HIR::Operation& op) {
        op.print(os);
        if (op.dst_vars.size() > 0) {
            os << " dst:";
            for (auto& dst : op.dst_vars) {
                os << " " << dst.get();
            }
        }
        os << " args:";
        for (auto& a : op.args) {
            os << " " << a.get();
        }
    };
    partition_result.pre->print(file, printer);
    file << std::endl;
    file.close();

    file.open("part-CPU.hir", std::ios::trunc);
    partition_result.cpu->print(file);
    file << std::endl;
    file.close();

    file.open("part-POST.hir", std::ios::trunc);
    partition_result.post->print(file);
    file << std::endl;
    file.close();
    
    return 0;
}