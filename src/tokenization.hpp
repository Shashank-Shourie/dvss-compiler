#pragma once

#include<string>
#include<optional>
#include<vector>
#include<iostream>

enum class TokenType{
    exit,
    int_lit,
    semi,
    open_paren,
    close_paren,
    identifier,
    let,
    eq
};

struct Token{
    TokenType type;
    std::optional<std::string> value{};
};

/**Creates tokens from the file */
class Tokenizer{
public:
    inline explicit Tokenizer(const std::string& src):m_src(src){}

    inline std::vector<Token> tokenize(){

        std::vector<Token> tokens;

        std::string buff;

        while(peek().has_value()){
            // std::cout<<peek().value()<<"\n";
            if(std::isalpha(peek().value())){
                // std::cout<<"D1"<<std::endl;
                buff.push_back(consume());
                // std::cout<<buff<<"\n";
                while(peek().has_value() && std::isalnum(peek().value())){

                    // std::cout<<"D2"<<peek().value()<<std::endl;
                    buff.push_back(consume());
                    // std::cout<<buff<<"\n";
                }
                if(buff=="exit"){
                    tokens.push_back({.type=TokenType::exit});
                    buff.clear();
                    continue;
                }else if(buff=="let"){
                    tokens.push_back({.type=TokenType::let});
                    buff.clear();
                    continue;
                }else{
                    // std::cout<<buff<<"\n";
                    tokens.push_back({.type=TokenType::identifier, .value = buff});
                    buff.clear();
                    continue;
                }
            }else if(std::isdigit(peek().value())){
                buff.push_back(consume());
                while(peek().has_value()&&std::isdigit(peek().value())){
                    // std::cout<<"D3"<<std::endl;
                    buff.push_back(consume());
                }
                // std::cout<<buff<<"\n";
                tokens.push_back({.type=TokenType::int_lit, .value=buff});
                buff.clear();
                continue;
            }else if(peek().value() == '('){
                consume();
                tokens.push_back({.type=TokenType::open_paren});
            }else if(peek().value() == ')'){
                consume();
                tokens.push_back({.type=TokenType::close_paren});
            }else if(peek().value() == '='){
                consume();
                tokens.push_back({.type=TokenType::eq});
            }else if(peek().value() == ';'){
                consume();
                tokens.push_back({.type=TokenType::semi});
                buff.clear();
                continue;
            }else if(std::isspace(peek().value())){
                consume();
                buff.clear(); 
                continue;
            }else{

                std::cerr<<"Wrong Syntax cuh"<<std::endl;
                exit(EXIT_FAILURE);
            }
        }
        m_index = 0;
        // std::cout<<"return";
        return tokens;
    }

private:
    [[nodiscard]] inline std::optional<char> peek(int offset = 0) const{
        if(m_index+offset >= m_src.length()){
            return {};
        }else{
            return m_src.at(m_index+offset);
        }
    }

    inline char consume(){
        return m_src.at(m_index++);
    }

    const std::string m_src;
    size_t m_index=0;
};