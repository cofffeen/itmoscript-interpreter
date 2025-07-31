#include <iostream>
#include <sstream>
#include <fstream>

#include <lib/interpreter.cpp>

int main(int argc, char **argv) {
    std::string code;
    std::string file_name = argv[1];

    std::ifstream fin(file_name);

    char c;
    while(fin.get(c)){
        code.push_back(c);
    }

    std::istringstream input(code);
    std::ostringstream output;

    interpret(input, output);

    std::cout << output.str();

    return 0;
}