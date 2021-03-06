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
//    map<int, map<string, Variable> > variables;
    
    vector<string> mips_instructions;
    
    // usable registers(stored in a stack)
    vector<int> regs = {7,6,5,4,3,2,1,0};
    
    // the status of registers : (variable -> register)
    // for example : regs_status["temp1"] = 1 means register [1] is used by temporary variable "temp1"
    map<string, int> regs_status;
    
    CodeGenerator(const vector<Instruction> instructions):instructions(instructions){}
    
    int get_offset(string offset){
//        pair<string, int> p = Instruction::unseriablize(variable_name_scope);
//        string variable_name = p.first;
//        int scope = p.second;
//        return variables[scope][variable_name].offset;
        return stoi(offset);
    }
    
    int get_reg(string temp){
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
        mips_instructions.push_back(".data\nprompt: .asciiz \"enter an integer : \"\nend: .asciiz \"\\n\"\n\n.text\n");
        for(auto instruction : instructions){
            translate(instruction);
        }
        mips_instructions.push_back("\nread:\nli $v0 4\nla $a0 prompt\nsyscall\nli $v0 5\nsyscall\njr $ra\n");
        mips_instructions.push_back("\nwrite:\nli $v0 1\nsyscall\nli $v0 4\nla $a0 end\nsyscall\njr $ra\n");
    }
    
    void print_mips_instructions(){
        ofstream mips("mips.txt");
        for(auto mips_instruction : mips_instructions){
            mips << mips_instruction;
        }
        mips.close();
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
                s << "\n" << res << ":\n" ;
                if(res == "main_block"){
                    s << "\nmove $s0 $ra\n";
                }
                break;
            }
            
            case LOAD_IMMEDIATE:{
                int reg = get_reg(res);
                s << "li $t" << reg << " " << num1 << "\n";
                break;
            }
                
            case LOAD_VARIABLE:{
                int reg = get_reg(res);
                int offset = get_offset(num1);
                
                s << "lw $t" << reg << " " << offset << "($sp)" << "\n";
                break;
            }
            
            case __MUL__:{
                int reg2 = release_reg(num2);
                int reg1 = release_reg(num1);
                
                s << "mult $t" << reg1 << " " << "$t" << reg2 << "\n";
                int reg3 = get_reg(res);
                s << "mflo " << "$t" << reg3 << "\n";
                break;
            }
            
            case __DIV__:{
                int reg2 = release_reg(num2);
                int reg1 = release_reg(num1);
                
                s << "div $t" << reg1 << " " << "$t" << reg2 << "\n";
                int reg3 = get_reg(res);
                s << "mflo " << "$t" << reg3 << "\n";
                break;
            }
                
            case __ADD__:{
                int reg2 = release_reg(num2);
                int reg1 = release_reg(num1);
                
                int reg3 = get_reg(res);
                
                s << "add $t" << reg3 << " " << "$t" << reg1 << " " << "$t" << reg2 << "\n";
                break;
            }
                
            case __SUB__:{
                int reg2 = release_reg(num2);
                int reg1 = release_reg(num1);
                
                int reg3 = get_reg(res);
                
                s << "sub $t" << reg3 << " " << "$t" << reg1 << " " << "$t" << reg2 << "\n";
                break;
            }
            
            case ASSIGN:{
                int offset = get_offset(res);
                if(num1=="0"){
                    s << "sw $zero" << " " << offset << "($sp)" << "\n";
                }
                else{
                    int reg = release_reg(num1);
                    s << "sw $t" << reg << " " << offset << "($sp)" << "\n";
                }
                
                break;
            }
                
            case GOTO:{
                s << "j " << res << "\n";
                break;
            }
                
            case GOTO_LINK:{
                s << "jal " << res << "\n";
                break;
            }
                
            case EXIT:{
                s << "li $v0 10\nsyscall" << "\n";
                break;
            }
                
            case __EQUAL__:{
                int reg2 = release_reg(num2);
                int reg1 = release_reg(num1);
                s << "beq $t" << reg1 << " " << "$t" << reg2 << " " << res << "\n";
                break;
            }
                
            case __GREATER__:{
                int reg2 = release_reg(num2);
                int reg1 = release_reg(num1);
                s << "bgt $t" << reg1 << " " << "$t" << reg2 << " " << res << "\n";
                break;
            }
                
            case __LESS__:{
                int reg2 = release_reg(num2);
                int reg1 = release_reg(num1);
                s << "blt $t" << reg1 << " " << "$t" << reg2 << " " << res << "\n";
                break;
            }
                
            case __GREATER_or_EQUAL__:{
                int reg2 = release_reg(num2);
                int reg1 = release_reg(num1);
                s << "bge $t" << reg1 << " " << "$t" << reg2 << " " << res << "\n";
                break;
            }
                
            case __LESS_or_EQUAL__:{
                int reg2 = release_reg(num2);
                int reg1 = release_reg(num1);
                s << "ble $t" << reg1 << " " << "$t" << reg2 << " " << res << "\n";
                break;
            }
                
            case __RETURN__:{
                int reg = release_reg(res);
                s << "move $v0 " << "$t" << reg << "\n";
                s << "move $ra $s0\n";
                s << "jr $ra\n";
                break;
            }
                
            case READ:{
                int offset = get_offset(res);
                s << "jal read\n";
                s << "sw $v0 " << offset << "($sp)\n";
                break;
            }
            
            case WRITE:{
                int reg = release_reg(res);
                s << "move $a0 $t" << reg << "\n";
                s << "jal write\n";
                break;
            }
                
            default:
                break;
        }
        mips_instructions.push_back(s.str());
    }
};


#endif /* CodeGenerator_h */
