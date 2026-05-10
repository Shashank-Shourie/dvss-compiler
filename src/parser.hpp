#pragma once

#include <variant>

#include "tokenization.hpp"

/** Integer value */
struct NodeExprIntLit{
    Token int_lit;
};

/** The identifier (variable name) */
struct NodeExprIdentifier{
    Token identifier;
};

struct NodeExpr{
    std::variant<NodeExprIntLit,NodeExprIdentifier> var;
};

struct NodeStmtExit{
    NodeExpr exp;
};

struct NodeStmtLet{
    Token identifier;
    NodeExpr exp;
};

struct NodeStmt{
    std::variant<NodeStmtExit, NodeStmtLet> var;
};

struct NodeProg{
    std::vector<NodeStmt> stmts;
};
    

/**  Creates a parse tree from tokens*/
class Parser{
public:
    inline explicit Parser(std::vector<Token> tokens) : m_tokens(std::move(tokens)){}

    std::optional<NodeExpr> parse_expr(){
        if(peek().has_value() && peek().value().type == TokenType::int_lit){
            return NodeExpr{.var = NodeExprIntLit{.int_lit=consume()}};
        }else if(peek().has_value() && peek().value().type == TokenType::identifier){
            return NodeExpr{.var = NodeExprIdentifier{.identifier = consume()}};
        }
        return {};
    }

    std::optional<NodeStmt> parse_stmt(){
        if(peek().value().type == TokenType::exit && peek(1).has_value() && peek(1).value().type == TokenType::open_paren){
            consume();
            consume();
            NodeStmtExit stmt_exit;
            if(auto node_expr = parse_expr()){
                stmt_exit = {.exp = node_expr.value()};
            }else{
                std::cerr<<"Invalid Expression"<<std::endl;
                exit(EXIT_FAILURE);
            }
            if(peek().has_value() && peek().value().type == TokenType::close_paren){
                consume();
            }else{
                std::cerr<<"Invalid Expression, expecting ')'"<<std::endl;
                exit(EXIT_FAILURE);
            }
            if(peek().has_value() && peek().value().type == TokenType::semi){
                consume();
            }else{
                std::cerr<<"Invalid Expression, expecting a semi colon"<<std::endl;
                exit(EXIT_FAILURE);
            }
            return NodeStmt{.var =stmt_exit};
        }else if(peek().has_value() && peek().value().type == TokenType::let && peek(1).has_value() && peek(1).value().type == TokenType::identifier && peek(2).has_value() && peek(2).value().type == TokenType::eq){
            consume();
            NodeStmtLet stmt_let = {.identifier=consume()};
            consume();
            if(auto expr = parse_expr()){
                stmt_let.exp = expr.value();
            }else{
                std::cerr<<"Invalid expression"<<std::endl;
                exit(EXIT_FAILURE);
            }
            if(peek().has_value() && peek().value().type == TokenType::semi){
                consume();
            }else{
                std::cerr<<"Expected ';'"<<std::endl;
                exit(EXIT_FAILURE);
            }
            return NodeStmt{.var = stmt_let};
        }else{
            return {};
        }
    }

    std::optional<NodeProg> parse_prog(){
        NodeProg prog;
        while(peek().has_value()){
            if(auto stmt = parse_stmt()){
                prog.stmts.push_back(stmt.value());
            }else{
                std::cerr<<"Invalid statement"<<std::endl;
                exit(EXIT_FAILURE);
            }
        }
        return prog;
    }
private:
    [[nodiscard]] inline std::optional<Token> peek(int offset = 0) const{
        if(m_index+offset >= m_tokens.size()){
            return {};
        }else{
            return m_tokens.at(m_index+offset);
        }
    }

    inline Token consume(){
        return m_tokens.at(m_index++);
    }

    const std::vector<Token> m_tokens;
    size_t m_index = 0;
};