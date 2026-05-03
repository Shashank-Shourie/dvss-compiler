#include <iostream>
#include <fstream>
#include <sstream>
#include <optional>
#include <vector>

#include "./tokenization.hpp"

std::string tokens_to_asm(const std::vector<Token>& tokens){
    std::stringstream output;
    output << "global _start\n_start:\n";
    for(int i=0;i<tokens.size();i++){
        const Token& token = tokens.at(i);
        if(token.type == TokenType::exit){
            if(i+1<tokens.size() && tokens.at(i+1).type == TokenType::int_lit){
                if(i+2<tokens.size() && tokens.at(i+2).type == TokenType::semi){
                    output<<"  mov rax, 60\n";
                    output<<"  mov rdi, "<< tokens.at(i+1).value.value()<<"\n";
                    output<<"  syscall";
                }else{
                    std::cerr<<"Give the exit value"<<std::endl;
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

    Tokenizer tokenizer(std::move(contents));

    std::vector<Token> tokens =  tokenizer.tokenize();

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