#include <iostream>
#include <fstream>
#include <sstream>
#include <optional>
#include <vector>

#include "./tokenization.hpp"
#include "./parser.hpp"
#include "./generation.hpp"


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

    Tokenizer tokenizer(std::move(contents));                               //A tokenizer class used for tokenizing the string

    std::vector<Token> tokens =  tokenizer.tokenize();                      //Creating a vector of tokens

    Parser parser(std::move(tokens));                                       //A parser class to create a parse tree from tokens

    std::optional<NodeProg> prog = parser.parse_prog();                     //Creating a parse tree from tokens

    if(!prog.has_value()){                                                  //To check if the tree has been creted
        std::cerr<<"Invalid Program"<<std::endl;
        exit(EXIT_FAILURE);
    }

    Generator generator(prog.value());                                      //Generator class to generate assembly from the parse tree

    // std::cout<<tokens_to_asm(tokens)<<std::endl;

    {
        std::fstream file("out.asm",std::ios::out);                         //Creates the output file to store the assembly code
        if(!file){
            std::cerr << "FAILED TO OPEN OUTPUT FILE\n";
            return EXIT_FAILURE;
        }
        file<<generator.gen_prog();                                         //Generates the assembly code
        // std::cout<<"Written";
    }

    system("nasm -felf64 out.asm");
    system("ld -o out out.o");

    return EXIT_SUCCESS;
}