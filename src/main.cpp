#include <iostream>
#include <fstream>
#include <sstream>
#include <optional>
#include <vector>

enum class TokenType{
    _return,
    int_lit,
    semi
};

struct Token{
    TokenType type;
    std::optional<std::string> value{};
};

std::vector<Token> tokenize(const std::string& str){
    std::vector<Token> tokens;

    std::string buff;
    for(int i=0;i<str.size();i++){
        char c = str.at(i);
        if(std::isalpha(c)){
            buff.push_back(c);
            i++;
            while(std::isalnum(str.at(i))){
                buff.push_back(str.at(i));
                i++;
            }
            i--;
            if(buff=="return"){
                tokens.push_back({.type = TokenType::_return});
                buff.clear();
            }else{
                std::cerr<<"It aint return cuh!"<<std::endl;
                exit(EXIT_FAILURE);
            }
        }else if(std::isdigit(c)){
            buff.push_back(c);
            i++;
            while(std::isdigit(str.at(i))){
                buff.push_back(str.at(i));
                i++;
            }
            i--;
            tokens.push_back({.type = TokenType::int_lit, .value = buff});
            buff.clear();
        }else if(std::isspace(c)){
            continue;
        }else if(c==';'){
            tokens.push_back({.type = TokenType::semi});
        }else{
            std::cerr<<"U messed up bruh"<<std::endl;
            exit(EXIT_FAILURE);
        }
    }
    return tokens;
}

std::string tokens_to_asm(const std::vector<Token>& tokens){
    std::stringstream output;
    output << "global _start\n_start:\n";
    for(int i=0;i<tokens.size();i++){
        const Token& token = tokens.at(i);
        if(token.type == TokenType::_return){
            if(i+1<tokens.size() && tokens.at(i+1).type == TokenType::int_lit){
                if(i+2<tokens.size() && tokens.at(i+2).type == TokenType::semi){
                    output<<"  mov rax, 60\n";
                    output<<"  mov rdi, "<< tokens.at(i+1).value.value()<<"\n";
                    output<<"  syscall";
                }else{
                    std::cerr<<"Give the return value"<<std::endl;
                    exit(EXIT_FAILURE);
                }
            }
        }
    }
    return output.str();
}

int main(int argc, char* argv[]){
    if(argc!=2){
        std::cerr<<"Incorrect Usage. Try "<<std::endl;
        std::cerr<<"dvss <example.dvss>"<<std::endl;
        return EXIT_FAILURE;
    }
    std::stringstream contents_stream;

    {
        std::fstream input(argv[1],std::ios::in);                           //Creating a file pointer pointing to the file given as the argument
        contents_stream << input.rdbuf();                                   //Sending the contents read from the read buffer to the string stream
    }                                                                       //by opening the file in different scope, the file auto closes without needing to explicitly call input.close()

    std::string contents = contents_stream.str();                           //Converting the string stream to string

    std::vector<Token> tokens =  tokenize(contents);

    // std::cout<<tokens_to_asm(tokens)<<std::endl;

    {
        std::fstream file("out.asm",std::ios::out);
        if(!file){
            std::cerr << "FAILED TO OPEN OUTPUT FILE\n";
            return EXIT_FAILURE;
        }
        file<<tokens_to_asm(tokens);
        // std::cout<<"Written";
    }

    system("nasm -felf64 out.asm");
    system("ld -o out out.o");

    return EXIT_SUCCESS;
}