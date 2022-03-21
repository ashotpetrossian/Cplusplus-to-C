#ifndef PARSER_H_
#define PARSER_H_

#include <iostream>
#include <vector>
#include <fstream>
#include <iterator>
#include <algorithm>

#include "Tokenizer.h"

std::vector<std::string> to_fout; //everything goes here by push back

void writing_to_fout (std::vector<std::string> to_fout) {
    
    std::ofstream fout;
    std::string output_file = "C.txt";
    fout.open(output_file);
    if (!fout.is_open()) {
        throw ( "Can't open the file\n" );
    }
    std::ostream_iterator<std::string, char> output_iterator(fout);
    std::copy(to_fout.begin(), to_fout.end(), output_iterator);
}

class Parser {
private:
    Tokenizer t;
public:
    Parser();
    void struct_generator();
    void function_generator();
    void main_generator();
};

#endif