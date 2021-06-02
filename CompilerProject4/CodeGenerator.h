//
//  CodeGenerator.h
//  CompilerProject4
//
//  Created by PatrickDD on 2021/6/1.
//

#ifndef CodeGenerator_h
#define CodeGenerator_h

#include "Instruction.h"
#include "Defines.h"

class CodeGenerator{
public:
    vector<Instruction> instructions;
    map<int, map<string, Variable> > variables;
    
    vector<string> mips_instructions;
    vector<int> regs = {7,6,5,4,3,2,1,0};
    map<string, int> regs_status;
    
    CodeGenerator(const vector<Instruction> instructions,
                  const map<int, map<string, Variable> >& variables):
                        instructions(instructions), variables(variables){}
    
    int get_offset(string variable_name_scope){
        pair<string, int> p = Instruction::unseriablize(variable_name_scope);
        string variable_name = p.first;
        int scope = p.second;
        return variables[scope][variable_name].offset;
    }
    
    int get_register(string temp){
        int reg = regs.back();
        regs.pop_back();
        regs_status[temp] = reg;
        return reg;
    }
    
    int release_reg(string temp){
        int reg = regs_status[temp];
        regs_status.erase(temp);
        regs.push_back(reg);
        return reg;
    }
    
    void translate(){
        mips_instructions.push_back(".data\nprompt: .asciiz \"enter an integer : \"\nprint: .asciiz \"result : \"\nend: .asciiz \"\\n\"\n\n.text");
        for(auto instruction : instructions){
            translate(instruction);
        }
        mips_instructions.push_back("\nread:\nli $v0 4\nla $a0 prompt\nsyscall\nli $v0 5\nsyscall\njr $ra");
        mips_instructions.push_back("\nwrite:\nli $v0 4\nla $a0 print\nsyscall\nli $v0 1\nsyscall\nli $v0 4\nla $a0 end\nsyscall\njr $ra");
    }
    
    void print_mips_instructions(){
        for(auto mips_instruction : mips_instructions){
            cout << mips_instruction << endl;
        }
    }
    
    void translate(Instruction instruction){
        
        ostringstream s;
        
        int Op = instruction.Op;
        string num1 = instruction.num1;
        string num2 = instruction.num2;
        string res = instruction.res;
        
        bool is_label = false;
        
        switch (Op) {
            case LABEL:{
                s << "\n" << res << ":" ;
                if(res == "main_block"){
                    s << "\nmove $s0 $ra";
                }
                break;
            }
            
            case LOAD_IMMEDIATE:{
                int reg = get_register(res);
                s << "li $t" << reg << " " << num1;
                break;
            }
                
            case LOAD_VARIABLE:{
                int reg = get_register(res);
                int offset = get_offset(num1);
                
                s << "lw $t" << reg << " " << offset << "($sp)";
                break;
            }
            
            case __MUL__:{
                int reg2 = release_reg(num2);
                int reg1 = release_reg(num1);
                
                s << "mult $t" << reg1 << " " << "$t" << reg2 << "\n";
                int reg3 = get_register(res);
                s << "mflo " << "$t" << reg3;
                break;
            }
            
            case __DIV__:{
                int reg2 = release_reg(num2);
                int reg1 = release_reg(num1);
                
                s << "div $t" << reg1 << " " << "$t" << reg2 << "\n";
                int reg3 = get_register(res);
                s << "mflo " << "$t" << reg3;
                break;
            }
                
            case __ADD__:{
                int reg2 = release_reg(num2);
                int reg1 = release_reg(num1);
                
                int reg3 = get_register(res);
                
                s << "add $t" << reg3 << " " << "$t" << reg1 << " " << "$t" << reg2;
                break;
            }
                
            case __SUB__:{
                int reg2 = release_reg(num2);
                int reg1 = release_reg(num1);
                
                int reg3 = get_register(res);
                
                s << "sub $t" << reg3 << " " << "$t" << reg1 << " " << "$t" << reg2;
                break;
            }
            
            case ASSIGN:{
                int offset = get_offset(res);
                if(num1=="0"){
                    s << "sw $zero" << " " << offset << "($sp)";
                }
                else{
                    int reg = release_reg(num1);
                    s << "sw $t" << reg << " " << offset << "($sp)";
                }
                
                break;
            }
                
            case GOTO:{
                s << "j " << res;
                break;
            }
                
            case GOTO_LINK:{
                s << "jal " << res;
                break;
            }
                
            case EXIT:{
                s << "li $v0 10\nsyscall";
                break;
            }
                
            case __EQUAL__:{
                int reg2 = release_reg(num2);
                int reg1 = release_reg(num1);
                s << "beq $t" << reg1 << " " << "$t" << reg2 << " " << res;
                break;
            }
                
            case __GREATER__:{
                int reg2 = release_reg(num2);
                int reg1 = release_reg(num1);
                s << "bgt $t" << reg1 << " " << "$t" << reg2 << " " << res;
                break;
            }
                
            case __LESS__:{
                int reg2 = release_reg(num2);
                int reg1 = release_reg(num1);
                s << "blt $t" << reg1 << " " << "$t" << reg2 << " " << res;
                break;
            }
                
            case __GREATER_or_EQUAL__:{
                int reg2 = release_reg(num2);
                int reg1 = release_reg(num1);
                s << "bge $t" << reg1 << " " << "$t" << reg2 << " " << res;
                break;
            }
                
            case __LESS_or_EQUAL__:{
                int reg2 = release_reg(num2);
                int reg1 = release_reg(num1);
                s << "ble $t" << reg1 << " " << "$t" << reg2 << " " << res;
                break;
            }
                
            case __RETURN__:{
                int reg = release_reg(res);
                s << "move $v0 " << "$t" << reg << "\n";
                s << "move $ra $s0\n";
                s << "jr $ra";
                break;
            }
                
            default:
                break;
        }
        mips_instructions.push_back(s.str());
    }
};


#endif /* CodeGenerator_h */
