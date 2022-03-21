#include "Parser.h"

Parser::Parser() {
    t.class_detection();
    t.data_detection();
    t.function_detection();
    t.virtual_function_detection();
    t.function_return_type_detection();
    t.function_argument_list_detection();
    t.main_detection();
}

void Parser::struct_generator() {
    auto text = t.get_text();
    std::string structs;
    for (size_t i = 0; i < t.all_classes.size(); i++)
    {
        structs += "\nstruct ";
        structs += t.all_classes[i].first;
        if (t.is_base(t.all_classes[i].first)) {
            structs += "\n";
        } else {
            structs += " : public ";
            structs += t.all_classes[i].second;
            structs += "\n";
        }
        structs += "{\n";
        
        std::vector<std::vector<std::string>> data_lines;
        for (size_t m = 0; m < text.size(); m++)
        {
            auto class_identifier = std::find(text[m].begin(), text[m].end(), "class");
            std::string class_name;
            if (class_identifier != text[m].end()) {
                std::string class_name = *(class_identifier + 1);
            
                if (class_name == t.all_classes[i].first) { 
                    for (size_t j = m + 1; j < text.size(); j++)
                    {
                        bool no_this_in_function_body = false;
                        for (size_t k = 0; k < text[j].size(); k++)
                        {
                            if (text[j][k] == "}") { no_this_in_function_body = true; }
                        }
                        auto data_member_line = std::find(text[j].begin(), text[j].end(), "=");
                        if (data_member_line != text[j].end() && !no_this_in_function_body) {
                                data_lines.push_back(text[j]);                           
                            }
                        if (std::find(text[j].begin(), text[j].end(), "class") != text[j].end()) {  break; }
                        if (std::find(text[j].begin(), text[j].end(), "main") != text[j].end()) { break; }
                    }
                }
            }  
        }
        for (size_t i = 0; i < data_lines.size(); i++)
        {
            for (size_t j = 0; j < data_lines[i].size(); j++)
            {
                structs += data_lines[i][j];
                structs += " ";
                if (data_lines[i][j] == ";") {
                    structs += "\n";
                }                
            }            
        }

        if (t.is_derived(t.all_classes[i].first)) {
            structs += "\t";
            structs += t.all_classes[i].second;
            structs += " subobject;\n";
        }

        data_lines.clear();
        structs += "};\n";
    }

    to_fout.push_back(structs);
}

void Parser::function_generator () {
    std::string functions;
    std::vector<std::pair<std::pair<std::string,std::string>, std::vector<std::string>>> function_lines; //class name | function name | whole line
    auto text = t.get_text();
    for (size_t i = 0; i < text.size(); i++)
    {
        std::pair<std::pair<std::string,std::string>, std::vector<std::string>> tmp;
        auto class_identifier = std::find(text[i].begin(), text[i].end(), "class");
        if (class_identifier != text[i].end()) {
            std::string class_name;
            class_name = *(class_identifier + 1);
            tmp.first.first = class_name;
        }
        for (size_t j = i + 1; j < text.size(); j++)
        {
            std::string the_function;
            auto function_line = std::find(text[j].begin(), text[j].end(), "(");
            if (function_line != text[j].end()) {
                the_function = *(function_line - 1);
                tmp.first.second = the_function;
                for (size_t k = 0; k < text[j].size(); k++)
                {
                    if (text[j][k] != "virtual") {
                        tmp.second.push_back(text[j][k]);
                    }
                }
                if (!tmp.first.first.empty()) {
                    function_lines.push_back(tmp);
                }
            }
            tmp.second.clear();
            if (std::find(text[j].begin(), text[j].end(), "class") != text[j].end()) { break; }
            if (std::find(text[j].begin(), text[j].end(), "main") != text[j].end()) { i = j + 1; break; }
        }
        tmp.first.first.clear();
    }  

    for (size_t i = 0; i < function_lines.size(); i++)
    {
        for (int j = 0; j < function_lines[i].second.size(); ++j) {
            if (function_lines[i].second[j] == "(") {
                function_lines[i].second[j] = "( " + function_lines[i].first.first + "* t ";
                for (size_t k = 0; k < t.function_args_list.size(); k++)
                {
                    if (t.function_args_list[k].first == function_lines[i].first.first
                        && t.function_args_list[k].second.first == function_lines[i].first.second) {
                        function_lines[i].second[j] += ",";
                    }
                }
            }
        }
    }    

    for (size_t i = 0; i < function_lines.size(); i++)
    {
        for (size_t j = 0; j < function_lines[i].second.size(); j++)
        {
            functions += function_lines[i].second[j];
            functions += " ";          
        }        
        functions += "\n";         
    }
      
    to_fout.push_back(functions);
}

void Parser::main_generator() {
    std::string main_lines_C;
    std::string the_object_that_is_pointed;
    std::string class_type_for_new;
    for (size_t i = 0; i < t.main_lines.size(); i++)
    {
        for (size_t j = 0; j < t.main_lines[i].size(); j++)
        {
            int dot_found;
            int p_operator;
            int address_found;
            std::string the_object;
            std::string the_pointer_to_object;
            std::string function_or_data;
            std::string the_data;
            std::string the_function;
            for (size_t k = 0; k < t.main_lines[i][j].size(); k++)
            {
                if (t.main_lines[i][j][k] == '.') {
                    dot_found = k;
                    for (size_t n = 0; n < t.main_lines[i][j].size() && t.main_lines[i][j][n] != '.'; n++)
                    {
                        the_object.push_back(t.main_lines[i][j][n]);
                    }
                    for (size_t n = dot_found + 1; n < t.main_lines[i][j].size() && t.main_lines[i][j][n] != '('; n++)
                    {
                        function_or_data.push_back(t.main_lines[i][j][n]);
                    }  
                    for (size_t n = 0; n < t.all_functions.size(); n++)
                    {
                        if (t.all_functions[n].second == function_or_data && !the_object.empty()) {
                            the_function = function_or_data;
                            t.main_lines[i][j] = the_function + " ( &" + the_object + " "; 
                            for (size_t n = 0; n < t.function_args_list.size(); n++)
                            {
                                if (t.function_args_list[n].second.first == the_function) {
                                    t.main_lines[i][j] += ",";
                                } 
                            }
                            the_function.clear(); the_object.clear();   
                        }
                    }                                
                } 
               
                if (t.main_lines[i][j][k] == '&') {
                    if (!the_object_that_is_pointed.empty()) {the_object_that_is_pointed.clear();}
                    for (size_t n = k + 1; n < t.main_lines[i][j].size() && t.main_lines[i][j][n] != ' '; n++)
                    {
                        the_object_that_is_pointed.push_back(t.main_lines[i][j][n]);
                    }                    
                }

                if (t.main_lines[i][j] == "new") {
                    if (!class_type_for_new.empty()) {class_type_for_new.clear();}
                    for (size_t n = 0; n < t.main_lines[i][j + 1].size() && t.main_lines[i][j + 1][n] != '('; n++)
                    {
                        class_type_for_new.push_back(t.main_lines[i][j + 1][n]);
                    }
                }

                if (t.main_lines[i][j][k] == '-') {
                    p_operator = k;
                    for (size_t n = 0; n < t.main_lines[i][j].size() && t.main_lines[i][j][n] != '-'; n++)
                    {
                        the_pointer_to_object.push_back(t.main_lines[i][j][n]);
                    }
                    for (size_t n = p_operator + 2; n < t.main_lines[i][j].size() && t.main_lines[i][j][n] != '('; n++)
                    {
                        function_or_data.push_back(t.main_lines[i][j][n]);
                    }  
                    for (size_t n = 0; n < t.all_functions.size(); n++)
                    {
                        if (t.all_functions[n].second == function_or_data && !the_pointer_to_object.empty() 
                            && !the_object_that_is_pointed.empty() && class_type_for_new.empty()) {
                            the_function = function_or_data;
                            t.main_lines[i][j] = the_function + " ( &" + the_object_that_is_pointed + " "; 
                            for (size_t n = 0; n < t.function_args_list.size(); n++)
                            {
                                if (t.function_args_list[n].second.first == the_function) {
                                    t.main_lines[i][j] += ",";
                                } 
                            }
                            the_object_that_is_pointed.clear();
                        }
                        else if (t.all_functions[n].second == function_or_data && !the_pointer_to_object.empty() 
                                && !class_type_for_new.empty()) {
                            the_function = function_or_data;
                            t.main_lines[i][j] = class_type_for_new + " " + class_type_for_new + "_tmp_obj ;\n   " + the_function + " ( & " + class_type_for_new + "_tmp_obj" + " "; 
                            for (size_t n = 0; n < t.function_args_list.size(); n++)
                            {
                                if (t.function_args_list[n].second.first == the_function) {
                                    t.main_lines[i][j] += ",";
                                } 
                            }
                            class_type_for_new.clear();
                        }
                    }
                }          
            }                        

            main_lines_C += t.main_lines[i][j];
            main_lines_C += " ";
        }
        main_lines_C += "\n";
        
    }
    to_fout.push_back(main_lines_C);
    
} 

int main()
{
    Parser p;
    p.struct_generator();
    p.function_generator();
    p.main_generator();
    writing_to_fout(to_fout);      
}