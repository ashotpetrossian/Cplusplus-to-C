#include "Tokenizer.h"

Tokenizer::Tokenizer() {
    std::string path = "Cplusplus.txt";
    std::ifstream fin;
    fin.open(path);

    if (!fin.is_open()) {
        throw ( "Can't open the file.\n" );
    }
    std::vector<std::string> lines;
    
    while (!fin.eof()) {
        std::string line;
        std::getline(fin, line);
        std::string tmp;
        for (size_t i = 0; i <= line.size(); i++)
        {
            if (line[i] != ' ' && line[i] != '\0') {
                tmp += line[i];
            } else if (line[i - 1] != '\0') {
                lines.push_back(tmp);
                tmp = "";
            }
            if (line[i] == '\0') {
                text.push_back(lines);
                lines.clear();
            }
        }        
    }
}

void Tokenizer::class_detection() {
    auto text = get_text();
    for (size_t i = 0; i < text.size(); i++)
    {
        auto class_identifier = std::find(text[i].begin(), text[i].end(), "class");
        if (  class_identifier != text[i].end()) {

            if ( text[i].size() > 3) {     
                std::pair<std::string, std::string> tmp;
                tmp.first = *(class_identifier + 1)  ; tmp.second = *(class_identifier + 4);            
                all_classes.push_back(tmp);
            } else {
                std::pair<std::string, std::string> tmp;
                tmp.first = *(class_identifier + 1);
                tmp.second = "base";
                all_classes.push_back(tmp);
            }
        }        
    }    
}

bool Tokenizer::is_base(std::string class_specifier) {
    for (size_t i = 0; i < all_classes.size(); i++)
    {
        if (class_specifier == all_classes[i].first && all_classes[i].second == "base") {
            return true;
        }        
    }
    return false;    
}

bool Tokenizer::is_derived(std::string class_specifier) {
    for (size_t i = 0; i < all_classes.size(); i++)
    {
        if (class_specifier == all_classes[i].first && all_classes[i].second != "base") {
            return true;
        }        
    }
    return false;
}

void Tokenizer::data_detection() {
    auto text = get_text();
    for (size_t i = 0; i < text.size(); i++)
    {
        std::pair<std::string, std::string> tmp;
        auto class_identifier = std::find(text[i].begin(), text[i].end(), "class");
        if (class_identifier != text[i].end()) {
            std::string class_name = *(class_identifier + 1);
            tmp.first = class_name;
        }
        for (size_t j = i + 1; j < text.size(); j++)
        {
            auto data_member_line = std::find(text[j].begin(), text[j].end(), "=");
            if (data_member_line != text[j].end()) {                
                std::string data_name = *(data_member_line - 1);
                tmp.second = data_name;
                if (!tmp.first.empty()) {
                    all_data.push_back(tmp);
                }
            }
            if (std::find(text[j].begin(), text[j].end(), "class") != text[j].end()) { break; }
            if (std::find(text[j].begin(), text[j].end(), "main") != text[j].end()) { i = j - 1; break; }
        }
        tmp.first.clear();tmp.second.clear();        
    }    
}

bool Tokenizer::data_is_private(const std::string& variable) {
    auto text = get_text();
    for (size_t i = 0; i < text.size(); i++)
    {
        auto visibility_identifier = std::find(text[i].begin(), text[i].end(), "private");
        if (visibility_identifier != text[i].end()) {
            for (size_t j = i + 1; j < text.size(); j++)
            {
                auto data_member_line = std::find(text[j].begin(), text[j].end(), "=");
                if (data_member_line != text[j].end()) {
                    if (*(data_member_line - 1) == variable) { return true; }
                }
                if (text[j][0] == "protected" || text[j][0] == "public" || text[j][0] == "};") {
                    break;
                }
            }            
        }
    }
    return false;
}

bool Tokenizer::data_is_protected(const std::string& variable) {
    auto text = get_text();
    for (size_t i = 0; i < text.size(); i++)
    {
        auto visibility_identifier = std::find(text[i].begin(), text[i].end(), "protected");
        if (visibility_identifier != text[i].end()) {
            for (size_t j = i + 1; j < text.size(); j++)
            {
                auto data_member_line = std::find(text[j].begin(), text[j].end(), "=");
                if (data_member_line != text[j].end()) {
                    if (*(data_member_line - 1) == variable) { return true; }
                }
                if (text[j][0] == "private" || text[j][0] == "public" || text[j][0] == "};") {
                    break;
                }
            }            
        }
    }
    return false;
}

bool Tokenizer::data_is_public(const std::string& variable) {
    auto text = get_text();
    for (size_t i = 0; i < text.size(); i++)
    {
        auto visibility_identifier = std::find(text[i].begin(), text[i].end(), "public");
        if (visibility_identifier != text[i].end()) {
            for (size_t j = i + 1; j < text.size(); j++)
            {
                auto data_member_line = std::find(text[j].begin(), text[j].end(), "=");
                if (data_member_line != text[j].end()) {
                    if (*(data_member_line - 1) == variable) { return true; }
                }
                if (text[j][0] == "private" || text[j][0] == "protected" || text[j][0] == "};") {
                    break;
                }
            }            
        }
    }
    return false;
}

void Tokenizer::function_detection() {
    auto text = get_text();
    for (size_t i = 0; i < text.size(); i++)
    {
        std::pair<std::string, std::string> tmp;
        auto class_identifier = std::find(text[i].begin(), text[i].end(), "class");
        if (class_identifier != text[i].end()) {
            std::string class_name = *(class_identifier + 1);
            tmp.first = class_name;
        }
        for (size_t j = i + 1; j < text.size(); j++)
        {
            auto function_line = std::find(text[j].begin(), text[j].end(), "(");
            if (function_line != text[j].end()) {
                std::string function_name = *(function_line - 1);
                tmp.second = function_name;
                if (!tmp.first.empty()) {
                    all_functions.push_back(tmp);
                }
            }
            if (std::find(text[j].begin(), text[j].end(), "class") != text[j].end()) { break; }
            if (std::find(text[j].begin(), text[j].end(), "main") != text[j].end()) { i = j + 1; break; }
        }
        tmp.first.clear();tmp.second.clear(); 
    }    
}

bool Tokenizer::function_is_private(const std::string& function_member) {
    auto text = get_text();
    for (size_t i = 0; i < text.size(); i++)
    {
        auto visibility_identifier = std::find(text[i].begin(), text[i].end(), "private");
        if (visibility_identifier != text[i].end()) {
            for (size_t j = i + 1; j < text.size(); j++)
            {
                auto function_line = std::find(text[j].begin(), text[j].end(), "(");
                if (function_line != text[j].end()) {
                    if (*(function_line - 1) == function_member) { return true; }
                }
                if (text[j][0] == "protected" || text[j][0] == "public" || text[j][0] == "};") {
                    break;
                }
            }            
        }
    }
    return false;
}

bool Tokenizer::function_is_protected(const std::string& function_member) {
    auto text = get_text();
    for (size_t i = 0; i < text.size(); i++)
    {
        auto visibility_identifier = std::find(text[i].begin(), text[i].end(), "protected");
        if (visibility_identifier != text[i].end()) {
            for (size_t j = i + 1; j < text.size(); j++)
            {
                auto function_line = std::find(text[j].begin(), text[j].end(), "(");
                if (function_line != text[j].end()) {
                    if (*(function_line - 1) == function_member) { return true; }
                }
                if (text[j][0] == "private" || text[j][0] == "public" || text[j][0] == "};") {
                    break;
                }
            }            
        }
    }
    return false;
}

bool Tokenizer::function_is_public(const std::string& function_member) {
    auto text = get_text();
    for (size_t i = 0; i < text.size(); i++)
    {
        auto visibility_identifier = std::find(text[i].begin(), text[i].end(), "public");
        if (visibility_identifier != text[i].end()) {
            for (size_t j = i + 1; j < text.size(); j++)
            {
                auto function_line = std::find(text[j].begin(), text[j].end(), "(");
                if (function_line != text[j].end()) {
                    if (*(function_line - 1) == function_member) { return true; }
                }
                if (text[j][0] == "private" || text[j][0] == "protected" || text[j][0] == "};") {
                    break;
                }
            }            
        }
    }
    return false;
}

void Tokenizer::virtual_function_detection() {
    auto text = get_text();
    for (size_t i = 0; i < text.size(); i++)
    {
        std::pair<std::string, std::pair<std::string, std::string>> tmp;
        auto class_identifier = std::find(text[i].begin(), text[i].end(), "class");
        if (class_identifier != text[i].end()) {
            std::string class_name = *(class_identifier + 1);
            tmp.first = class_name;
        }

        for (size_t j = i + 1; j < text.size(); j++)
        {
            auto function_line = std::find(text[j].begin(), text[j].end(), "virtual");
            if (function_line != text[j].end()) {
                std::string function_name = *(function_line + 2);
                std::string type_name = *(function_line + 1);
                tmp.second.first = function_name;
                tmp.second.second = type_name;
                if (!tmp.first.empty()) {
                    v_table.push_back(tmp);
                    continue;
                }
            }

            auto function_line__for_virtual = std::find(text[j].begin(), text[j].end(), "(");
            if (function_line__for_virtual != text[j].end()) {
                std::string function_name = *(function_line__for_virtual - 1);
                std::string type_name = *(function_line__for_virtual - 2);
                if (is_derived(tmp.first)) { //if the "virtual" keyword has not been written but we still want it to push in v_table, as it is derived overloaded
                    for (size_t k = 0; k < v_table.size(); k++)
                    {
                        if (function_name == v_table[k].second.first) { //checking if we have the same function name in v_table, already pushed
                            for (size_t m = 0; m < all_classes.size(); m++)
                            {
                                //checking if the owner of previous(in comment) function, a class is itself a parent
                                if ( ((v_table[k].first == all_classes[m].second) && v_table[k].first!=tmp.first) || (is_base(v_table[k].first))) {
                                    if (type_name != v_table[k].second.second) {
                                        std::cerr<<"Return type is not identical or covariant to "<<v_table[k].second.second
                                            <<" of overriden virtual function!!\nTerminated!\n";
                                        exit(EXIT_FAILURE);
                                    }
                                    tmp.second.first = function_name;
                                    tmp.second.second = type_name;
                                    if (!tmp.first.empty() && v_table[k].second.second == type_name) {
                                        v_table.push_back(tmp);
                                        break;
                                    }
                                }
                            }    
                            break;                     
                        }
                    }                    
                }
            }

            if (std::find(text[j].begin(), text[j].end(), "class") != text[j].end()) { break; }
            if (std::find(text[j].begin(), text[j].end(), "main") != text[j].end()) { i = j - 1; break; }
        }
        tmp.first.clear();tmp.second.first.clear(); tmp.second.second.clear(); 
    }    
}

bool Tokenizer::function_is_virtual (const std::string& function_member) {
    for (size_t i = 0; i < v_table.size(); i++)
    {
        if (v_table[i].second.first == function_member) { return true; }
    }
    return false;
}

void Tokenizer::function_return_type_detection() {
    auto text = get_text();
    for (size_t i = 0; i < text.size(); i++)
    {
        std::pair<std::string, std::string> tmp;
        if (std::find(text[i].begin(), text[i].end(), "class") != text[i].end()) {
            for (size_t j = i + 1; j < text.size(); j++)
            {
                auto function_line = std::find(text[j].begin(), text[j].end(), "(");
                if (function_line != text[j].end()) {
                    std::string function_name = *(function_line - 1);
                    std::string function_return_type = *(function_line - 2);
                    tmp.first = function_name;
                    if (function_name == "main") { break; }
                    tmp.second = function_return_type;
                    function_return_types.push_back(tmp);
                    tmp.first.clear();tmp.second.clear();
                }
                if (std::find(text[j].begin(), text[j].end(), "class") != text[j].end()) { i = j - 1; break; }
                if (std::find(text[j].begin(), text[j].end(), "main") != text[j].end()) { i = j - 1; break; }
            } 
        }        
    }
}

bool Tokenizer::function_is_redifined_by_return_type_in_class(const std::string& function_name) {
    for (size_t i = 0; i < all_functions.size(); i++)
    {
        int count_of_function_with_the_same_name{0};
        if (all_functions[i].second == function_name) { count_of_function_with_the_same_name++; }
        for (size_t j = i + 1; j < all_functions.size(); j++)
        {
            if (all_functions[j].second == function_name) { count_of_function_with_the_same_name++; }
            if (all_functions[i].first == all_functions[j].first) {
                if (function_return_types[i].first == all_functions[i].second 
                    && ( count_of_function_with_the_same_name >= 2)
                    && ((function_return_types[i].first == function_name) 
                        || (function_return_types[j].first == function_name))) {
                        if (function_return_types[i].second != function_return_types[j].second) {
                            return true;
                        }
                    }
            }
        }        
    }
    return false;    
}

void Tokenizer::function_argument_list_detection() {
    auto text = get_text();
    for (size_t i = 0; i < text.size(); i++)
    {
        std::pair<std::string, std::pair<std::string, std::string>> tmp;
        auto class_identifier = std::find(text[i].begin(), text[i].end(), "class");
        if (class_identifier != text[i].end()) {
            std::string class_name = *(class_identifier + 1);
            tmp.first = class_name;
        }
        for (size_t j = i + 1; j < text.size(); j++)
        {
            auto function_args_start = std::find(text[j].begin(), text[j].end(), "(");
            auto function_args_end = std::find(text[j].begin(), text[j].end(), ")");
            if (function_args_start != text[j].end() && function_args_end != text[j].end()) {
                std::string function_args;
                std::string function_name = *(function_args_start - 1);
                for (auto i = function_args_start + 1; i != function_args_end; i++)
                {
                    function_args += (*(i) + " ");
                }                
                tmp.second.first = function_name;
                tmp.second.second = function_args;
                if (!tmp.second.second.empty() && !tmp.first.empty()) {
                    function_args_list.push_back(tmp);
                }
            }
            
            if (std::find(text[j].begin(), text[j].end(), "class") != text[j].end()) { break; }
            if (std::find(text[j].begin(), text[j].end(), "main") != text[j].end()) { break; }
        }
        tmp.first.clear();tmp.second.first.clear(); tmp.second.second.clear(); 
    }      
}

void Tokenizer::main_detection() {
    auto text = get_text();
    for (size_t i = 0; i < text.size(); i++)
    {
        auto main_identifier = std::find(text[i].begin(), text[i].end(), "main");
        if (main_identifier != text[i].end()) {
            for (size_t j = i; j < text.size(); j++)
            {
                main_lines.push_back(text[j]);
            }            
        }
    }

    for (size_t i = 0; i < main_lines.size(); i++)
    {
        for (size_t j = 0; j < all_classes.size(); j++)
        {
            auto class_type_identifier = std::find(main_lines[i].begin(), main_lines[i].end(), all_classes[j].first);
            if (class_type_identifier != main_lines[i].end()) {
                std::string class_name = *(class_type_identifier);
                std::string object_name = *(class_type_identifier + 1);
                std::pair<std::string, std::string> tmp;
                tmp.first = class_name;
                tmp.second = object_name;
                objects.push_back(tmp);
            }
        }  

        for (size_t j = 0; j < main_lines[i].size(); j++)
        {
            for (size_t k = 0; k < main_lines[i][j].size(); k++)
            {
                std::pair<std::string, std::string> tmp;
                std::string class_type_for_pointer;
                std::string pointer_to_object;
                if (main_lines[i][j][k] == '*') {
                    pointer_to_object = main_lines[i][j + 1];
                    for (size_t n = 0; n < main_lines[i][j].size() && main_lines[i][j][n] != '*'; n++)
                    {
                        class_type_for_pointer.push_back(main_lines[i][j][n]);
                    }
                    
                }
                if (!class_type_for_pointer.empty() && !pointer_to_object.empty()) {
                    tmp.first = class_type_for_pointer;
                    tmp.second = pointer_to_object;
                    pointers_to_objects.push_back(tmp);
                }
            }            
        }
        
        for (size_t j = 0; j < main_lines[i].size(); j++)
        {
            std::string pointer_used_for_call;
            std::string function_or_data;
            std::string called_function;
            std::string called_data;
            std::string p_operator = ">";
            int found_p_operator = main_lines[i][j].find(p_operator);
            if (found_p_operator != -1) {
                for (size_t k = found_p_operator + 1; k < main_lines[i][j].size() && 
                    (main_lines[i][j][k] != '(' || main_lines[i][j][k] != ';'); k++)
                {
                    function_or_data.push_back(main_lines[i][j][k]);                
                    for (size_t n = 0; n < all_functions.size(); n++)
                    {
                        if (function_or_data == all_functions[n].second) {
                            called_function = function_or_data;
                        }
                    }
                    for (size_t n = 0; n < all_data.size(); n++)
                    {
                        if (function_or_data == all_data[n].second) {
                            called_data = function_or_data;
                        }
                    } 
                }
                for (size_t m = 0; m < main_lines[i][j].size() && main_lines[i][j][m] != '-'; m++)
                {
                    pointer_used_for_call.push_back(main_lines[i][j][m]);
                }
                int pointer_found_in_pointers_to_objects {0};
                for (size_t n = 0; n < pointers_to_objects.size(); n++)
                {
                    if (pointer_used_for_call == pointers_to_objects[n].second) {pointer_found_in_pointers_to_objects++;}
                }
                if (pointer_found_in_pointers_to_objects == 0) { 
                    std::cerr<<"There is no such object defined!\n"
                        <<"object name: "<<pointer_used_for_call<<"\nTerminated!!!\n";
                    exit(EXIT_FAILURE);
                }
                if (!called_data.empty()) {
                    int if_the_pointer_can_call_that_data {0};
                    for (size_t n = 0; n < all_data.size(); n++)
                    {
                        for (size_t m = 0; m < pointers_to_objects.size(); m++)
                        {
                            if (data_is_private(called_data)) {
                                std::cerr<<"Trying to access private data\nTerminated\n";
                                exit(EXIT_FAILURE);
                            }
                            if (data_is_protected(called_data)) {
                                std::cerr<<"Trying to access protected data\nTerminated\n";
                                exit(EXIT_FAILURE);
                            }
                            if (all_data[n].first == pointers_to_objects[m].first && all_data[n].second == called_data
                                && pointer_used_for_call == pointers_to_objects[m].second) {
                                if_the_pointer_can_call_that_data++;
                            } 
                            else if (all_data[n].second == called_data
                                && pointer_used_for_call == pointers_to_objects[m].second) {
                                    if (is_derived(pointers_to_objects[m].first)){
                                        for (size_t k = 0; k < all_classes.size(); k++)
                                        {
                                            if (pointers_to_objects[m].first == all_classes[k].first) {
                                                std::string the_current_base = all_classes[k].second;
                                                for (size_t p = 0; p < all_data.size(); p++)
                                                {
                                                    if (all_data[p].first == the_current_base && all_data[p].second == called_data) {
                                                        if_the_pointer_can_call_that_data++;
                                                        break;
                                                    } else if (is_derived(the_current_base)) {
                                                        for (size_t q = 0; q < all_classes.size(); q++)
                                                        {
                                                            if (the_current_base == all_classes[q].first) {
                                                                std::string the_base_of_current_base = all_classes[q].second;
                                                                for (size_t r = 0; r < all_data.size(); r++)
                                                                {
                                                                    if (all_data[r].first == the_base_of_current_base  && all_data[r].second == called_data) {
                                                                    if_the_pointer_can_call_that_data++;
                                                                    break;
                                                                    }
                                                                }    
                                                            }
                                                                                                                    
                                                        }
                                                        
                                                    }
                                                }                                            
                                            }
                                        }
                                        
                                    }
                                }
                        }                    
                    }
        
                    if (if_the_pointer_can_call_that_data == 0) { 
                        std::cerr<<"There is no such data that \'"<<pointer_used_for_call
                            <<"\' can call, due to there is no such data in the class\nTerminated!!!\n";
                        exit(EXIT_FAILURE);
                    }  
                }  

                else if (!called_function.empty()) {
                    int if_the_pointer_can_call_that_function {0};
                    for (size_t n = 0; n < all_functions.size(); n++)
                    {
                        for (size_t m = 0; m < pointers_to_objects.size(); m++)
                        {
                            if (function_is_private(called_function)) {
                                    std::cerr<<"Trying to access private function\nTerminated\n";
                                    exit(EXIT_FAILURE);
                                }
                            if (function_is_protected(called_function)) {
                                std::cerr<<"Trying to access protected function\nTerminated\n";
                                exit(EXIT_FAILURE);
                            }
                            if (all_functions[n].first == pointers_to_objects[m].first && all_functions[n].second == called_function
                                && pointer_used_for_call == pointers_to_objects[m].second && !function_is_virtual(called_function)) {                                
                                if_the_pointer_can_call_that_function++;
                            } 
                            else if (all_functions[n].second == called_function
                                && pointer_used_for_call == pointers_to_objects[m].second && !function_is_virtual(called_function)) {
                                    if (is_derived(pointers_to_objects[m].first)) {
                                        for (size_t k = 0; k < all_classes.size(); k++)
                                        {
                                            if (pointers_to_objects[m].first == all_classes[k].first) {
                                                std::string the_current_base = all_classes[k].second;
                                                for (size_t p = 0; p < all_functions.size(); p++)
                                                {
                                                    if (all_functions[p].first == the_current_base) {
                                                        if_the_pointer_can_call_that_function++;
                                                        break;
                                                    } else if (is_derived(the_current_base)) {
                                                        for (size_t q = 0; q < all_classes.size(); q++)
                                                        {
                                                            if (the_current_base == all_classes[q].first) {
                                                                std::string the_base_of_current_base = all_classes[q].second;
                                                                for (size_t r = 0; r < all_functions.size(); r++)
                                                                {
                                                                    if (all_functions[r].first == the_base_of_current_base  && all_functions[r].second == called_function) {
                                                                    if_the_pointer_can_call_that_function++;
                                                                    break;
                                                                    }
                                                                }    
                                                            }                                                                                                                    
                                                        }
                                                    }
                                                }                                            
                                            }
                                        }                                        
                                    }
                                }
                            else if (all_functions[n].first == pointers_to_objects[m].first && all_functions[n].second == called_function
                                && pointer_used_for_call == pointers_to_objects[m].second && function_is_virtual(called_function)) {  
                                                              
                                if_the_pointer_can_call_that_function++;
                            } 
                        }                    
                    }
        
                    if (if_the_pointer_can_call_that_function == 0) { 
                        std::cerr<<"There is no such function that \'"<<pointer_used_for_call
                            <<"\' can call, due to there is no such function in the class\nTerminated!!!\n";
                        exit(EXIT_FAILURE);
                    }  
                }
            }
            called_data.clear();called_function.clear();function_or_data.clear();
        }
        

        for (size_t j = 0; j < main_lines[i].size(); j++)
        {
            std::string object_used_for_call;
            std::string function_or_data;
            std::string called_function;
            std::string called_data;
            std::string dot_operator = ".";
            int found_dot_index = main_lines[i][j].find(dot_operator);
            if (found_dot_index != -1) {
                for (size_t k = found_dot_index + 1; k < main_lines[i][j].size() 
                    && (main_lines[i][j][k] != '(' || main_lines[i][j][k] != ';'); k++)
                {
                    function_or_data.push_back(main_lines[i][j][k]);
                    for (size_t n = 0; n < all_functions.size(); n++)
                    {
                        if (function_or_data == all_functions[n].second) {
                            called_function = function_or_data;
                        }
                    }
                    for (size_t n = 0; n < all_data.size(); n++)
                    {
                        if (function_or_data == all_data[n].second) {
                            called_data = function_or_data;
                        }
                    }                    
                }
                for (size_t m = 0; m < main_lines[i][j].size() && main_lines[i][j][m] != '.'; m++)
                {
                    object_used_for_call.push_back(main_lines[i][j][m]);
                }
                
                int object_found_in_objects {0};
                for (size_t n = 0; n < objects.size(); n++)
                {
                    if (object_used_for_call == objects[n].second) {object_found_in_objects++;}
                }
                if (object_found_in_objects == 0) { 
                    std::cerr<<"There is no such object defined!\n"
                        <<"object name: "<<object_used_for_call<<"\nTerminated!!!\n";
                    exit(EXIT_FAILURE);
                }  
                
                if (!called_data.empty()) {
                    int if_the_object_can_call_that_data {0};
                    for (size_t n = 0; n < all_data.size(); n++)
                    {
                        for (size_t m = 0; m < objects.size(); m++)
                        {
                            if (data_is_private(called_data)) {
                                std::cerr<<"Trying to access private data\nTerminated\n";
                                exit(EXIT_FAILURE);
                            }
                            if (data_is_protected(called_data)) {
                                std::cerr<<"Trying to access protected data\nTerminated\n";
                                exit(EXIT_FAILURE);
                            }
                            if (all_data[n].first == objects[m].first && all_data[n].second == called_data
                                && object_used_for_call == objects[m].second) {
                                if_the_object_can_call_that_data++;
                            } 
                            else if (all_data[n].second == called_data
                                && object_used_for_call == objects[m].second) {
                                    if (is_derived(objects[m].first)){
                                        for (size_t k = 0; k < all_classes.size(); k++)
                                        {
                                            if (objects[m].first == all_classes[k].first) {
                                                std::string the_current_base = all_classes[k].second;
                                                for (size_t p = 0; p < all_data.size(); p++)
                                                {
                                                    if (all_data[p].first == the_current_base && all_data[p].second == called_data) {
                                                        if_the_object_can_call_that_data++;
                                                        break;
                                                    } else if (is_derived(the_current_base)) {
                                                        for (size_t q = 0; q < all_classes.size(); q++)
                                                        {
                                                            if (the_current_base == all_classes[q].first) {
                                                                std::string the_base_of_current_base = all_classes[q].second;
                                                                for (size_t r = 0; r < all_data.size(); r++)
                                                                {
                                                                    if (all_data[r].first == the_base_of_current_base  && all_data[r].second == called_data) {
                                                                    if_the_object_can_call_that_data++;
                                                                    break;
                                                                    }
                                                                }    
                                                            }
                                                                                                                    
                                                        }
                                                        
                                                    }
                                                }                                            
                                            }
                                        }
                                        
                                    }
                                }
                        }                    
                    }
        
                    if (if_the_object_can_call_that_data == 0) { 
                        std::cerr<<"There is no such data that \'"<<object_used_for_call
                            <<"\' can call, due to there is no such data in the class\nTerminated!!!\n";
                        exit(EXIT_FAILURE);
                    }  
                }  

                else if (!called_function.empty()) {
                    int if_the_object_can_call_that_function {0};
                    for (size_t n = 0; n < all_functions.size(); n++)
                    {
                        for (size_t m = 0; m < objects.size(); m++)
                        {
                            if (function_is_private(called_function)) {
                                    std::cerr<<"Trying to access private function\nTerminated\n";
                                    exit(EXIT_FAILURE);
                                }
                                if (function_is_protected(called_function)) {
                                    std::cerr<<"Trying to access protected function\nTerminated\n";
                                    exit(EXIT_FAILURE);
                                }
                            if (all_functions[n].first == objects[m].first && all_functions[n].second == called_function
                                && object_used_for_call == objects[m].second) {
                                
                                if_the_object_can_call_that_function++;
                            } 
                            else if (all_functions[n].second == called_function
                                && object_used_for_call == objects[m].second) {
                                    if (is_derived(objects[m].first)){
                                        for (size_t k = 0; k < all_classes.size(); k++)
                                        {
                                            if (objects[m].first == all_classes[k].first) {
                                                std::string the_current_base = all_classes[k].second;
                                                for (size_t p = 0; p < all_functions.size(); p++)
                                                {
                                                    if (all_functions[p].first == the_current_base) {
                                                        if_the_object_can_call_that_function++;
                                                        break;
                                                    } else if (is_derived(the_current_base)) {
                                                        for (size_t q = 0; q < all_classes.size(); q++)
                                                        {
                                                            if (the_current_base == all_classes[q].first) {
                                                                std::string the_base_of_current_base = all_classes[q].second;
                                                                for (size_t r = 0; r < all_functions.size(); r++)
                                                                {
                                                                    if (all_functions[r].first == the_base_of_current_base  && all_functions[r].second == called_function) {
                                                                    if_the_object_can_call_that_function++;
                                                                    break;
                                                                    }
                                                                }    
                                                            }
                                                                                                                    
                                                        }
                                                    }
                                                }                                            
                                            }
                                        }
                                        
                                    }
                                }
                        }                    
                    }
        
                    if (if_the_object_can_call_that_function == 0) { 
                        std::cerr<<"There is no such function that \'"<<object_used_for_call
                            <<"\' can call, due to there is no such function in the class\nTerminated!!!\n";
                        exit(EXIT_FAILURE);
                    }  
                }          
            }  
            called_data.clear();
            called_function.clear();
            function_or_data.clear();  
        }              
    }   
}