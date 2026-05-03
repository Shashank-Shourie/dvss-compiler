#pragma once

#include<string>
#include<optional>
#include<vector>
#include<iostream>

enum class TokenType{
    exit,
    int_lit,
    semi
};

struct Token{
    TokenType type;
    std::optional<std::string> value{};
};


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
                }else{
                    // std::cout<<buff<<"\n";
                    std::cerr<<"Wrong Syntax"<<std::endl;
                    exit(EXIT_FAILURE);
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
            }else if(peek().value() == ';'){
                consume();
                std::cout<<"Consumed"<<"\n";
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
    [[nodiscard]] std::optional<char> peek(int ahed = 0) const{
        if(m_index+ahed >= m_src.length()){
            return {};
        }else{
            return m_src.at(m_index+ahed);
        }
    }

    char consume(){
        return m_src.at(m_index++);
    }

    const std::string m_src;
    int m_index=0;
};