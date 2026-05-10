#pragma once

#include<sstream>
#include<unordered_map>

#include "./tokenization.hpp"
#include "./parser.hpp"


/**Generate assembly from parse tree */
class Generator{
public:
    inline explicit Generator(NodeProg prog):m_prog(std::move(prog)){};

    void gen_expr(const NodeExpr& expr){
        struct ExprVisitor{
            Generator* gen;
            void operator()(const NodeExprIntLit& expr_int_lit){
                gen->m_output << "  mov rax, " << expr_int_lit.int_lit.value.value() << "\n";
                gen->push("rax");
            }
            void operator()(const NodeExprIdentifier& expr_indetifier){
                if(!gen->m_vars.contains(expr_indetifier.identifier.value.value())){
                    std::cerr<<"Undeclared Identifier : "<<expr_indetifier.identifier.value.value()<<std::endl;
                    exit(EXIT_FAILURE);
                }
                const auto& var = gen->m_vars.at(expr_indetifier.identifier.value.value());
                std::stringstream offset;
                offset << "QWORD [rsp + " << (gen->m_stack_size - var.stack_loc-1)*8 << "]";
                gen->push(offset.str());
            }
        };

        ExprVisitor visitor{.gen = this};
        std::visit(visitor,expr.var);
    }

    void gen_stat(const NodeStmt& stmt){
        struct StmtVisitor{
            Generator* gen;
            void operator()(const NodeStmtExit& stmt_exit){
                gen->gen_expr(stmt_exit.exp);
                gen->m_output << "  mov rax, 60\n";
                gen->pop("rdi");
                gen->m_output << "  syscall\n";
            }
            void operator()(const NodeStmtLet& stmt_let){
                if(gen->m_vars.contains(stmt_let.identifier.value.value())){
                    std::cerr << "Identifier already used : "<<stmt_let.identifier.value.value()<<std::endl;
                    exit(EXIT_FAILURE);
                }
                gen->m_vars.insert({stmt_let.identifier.value.value(),Var{.stack_loc=gen->m_stack_size}});
                gen->gen_expr(stmt_let.exp);
            }
        };

        StmtVisitor visitor{.gen = this};
        std::visit(visitor,stmt.var);
    }

    [[nodiscard]] std::string gen_prog(){
        // std::stringstream output;
        m_output << "global _start\n_start:\n";

        for(const NodeStmt& stat : m_prog.stmts){
            gen_stat(stat);
        }

        m_output << "  mov rax, 60\n";
        m_output << "  mov rdi, 0\n";
        m_output << "  syscall";
        return m_output.str();
    }
private:
    void push(const std::string& reg){
        m_output << "  push " << reg << "\n";
        m_stack_size++;
    }
    void pop(const std::string& reg){
        m_output << "  pop " << reg << "\n";
        m_stack_size--;
    }

    struct Var{
        size_t stack_loc;
    };

    const NodeProg m_prog;
    std::stringstream m_output;
    size_t m_stack_size = 0;
    std::unordered_map<std::string,Var> m_vars{};
};