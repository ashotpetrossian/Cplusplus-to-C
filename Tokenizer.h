#ifndef TOKENIZER_H_
#define TOKENIZER_H_

#include <iostream>
#include <fstream>
#include <vector>
#include <iterator>
#include <algorithm>


class Tokenizer 
{
private:
    std::vector<std::vector<std::string>> text;
public:
    std::vector<std::pair<std::string, std::string>> all_classes; // class name | base class name or "no base"||"parent"
    std::vector<std::pair<std::string, std::string>> all_functions; // class name | function name
    std::vector<std::pair<std::string, std::string>> all_data; // class name | var name 
    std::vector<std::pair<std::string, std::pair<std::string,std::string>>> v_table; //class name | function name | return type
    std::vector<std::pair<std::string, std::string>> function_return_types; // function name | return type
    std::vector<std::pair<std::string, std::pair<std::string, std::string>>> function_args_list; // class name | function name | all args as string
    std::vector<std::vector<std::string>> main_lines; //all lines in raw view 
    std::vector<std::pair<std::string, std::string>> objects; // class name | object name
    std::vector<std::pair<std::string, std::string>> pointers_to_objects; // class name | pointer name
public:
    Tokenizer();
    auto get_text() const { return text; }
    void class_detection(); // sets all class names and if they are derived also saves the base class name
                            //if there is no base class the second of the pair sets to "base"
    bool is_base(std::string);
    bool is_derived(std::string);

    void function_detection();
    bool function_is_private(const std::string&);
    bool function_is_protected(const std::string&);
    bool function_is_public(const std::string&);

    void data_detection();
    bool data_is_private(const std::string&);
    bool data_is_protected(const std::string&);
    bool data_is_public(const std::string&);

    void virtual_function_detection();
    bool function_is_virtual(const std::string&);

    void function_return_type_detection();
    bool function_is_redifined_by_return_type_in_class(const std::string&);

    void function_argument_list_detection();

    void main_detection();

};

#endif