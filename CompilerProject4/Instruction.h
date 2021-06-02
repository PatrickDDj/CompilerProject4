//
//  Instruction.h
//  CompilerProject4
//
//  Created by PatrickDD on 2021/5/31.
//

#ifndef Instruction_h
#define Instruction_h

#include "Defines.h"

class Instruction{
public:
    int Op;
    string num1, num2, res;
    Instruction(int Op, string num1, string num2, string res):
        Op(Op), num1(num1), num2(num2), res(res){}
    
    static string seriablize(string variable_name, int scope){
        return variable_name + "_" + to_string(scope);
    }
    
    static pair<string, int> unseriablize(string variable_name_scope){
        int p = variable_name_scope.find("_");
        string variable_name = variable_name_scope.substr(0, p);
        int scope = stoi(variable_name_scope.substr(p+1));
        return make_pair(variable_name, scope);
    }
};


class InstructionManager{
public:
    vector<Instruction> instructions;
    int cur_temp=0;
    
    void add_instruction(int Op, string num1, string num2, string res){
        instructions.push_back(Instruction(Op, num1, num2, res));
    }
    
    
    
    void add_instruction(int Op, string num1, string num2, string res, int pos){
        instructions.insert(instructions.begin()+pos ,Instruction(Op, num1, num2, res));
    }

    void add_label(string label){
        instructions.back().res = label;
    }
    
    int get_pos(){
        return instructions.size();
    }
    
    void print_instructions(){
        for(auto instruction : instructions){
            printf("( %d, %s, %s, %s)\n", instruction.Op, instruction.num1.c_str(), instruction.num2.c_str(), instruction.res.c_str());
        }
    }
    
    string get_temp(){
        return "temp"+to_string(cur_temp++);
    }
    
    
    
};



#endif /* Instruction_h */
