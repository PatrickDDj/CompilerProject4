//
//  SemanticAnalyzer.h
//  CompilerProject3
//
//  Created by PatrickDD on 2021/5/12.
//

#include "Defines.h"
#include "Variable.h"
#include "Function.h"
#include "ScopeManager.h"
#include "Instruction.h"
#include "Node.h"

#ifndef SemanticAnalyzer_h
#define SemanticAnalyzer_h





class SemanticAnalyzer {
public:
    
    Node root;
    VariableManager variables;
    FunctionManager functions;
    ScopeManager scopes;
    InstructionManager instructions;
    
    
    SemanticAnalyzer(const Node& root):root(root){}
    
    
    
    void semantic_analysis(){
        Node& HEADER = root.sons[0];
        
        // jump into HEADER
        scopes.jump_into_Block(HEADER.id);
        
        proc_HEADER(HEADER);
        
        
        Node& MAIN = root.sons[1];
        
        
        proc_MAIN(MAIN);
        
        variables.print_variables();
//        functions.print_functions();
        
        instructions.print_instructions();
        
    }
    
    const vector<Instruction>& get_instructions(){
        return instructions.instructions;
    }
    
    const map<int, map<string, Variable> >& get_variables(){
        return variables.variables;
    }
    
    void proc_Block(const Node& Block){
        int scope = Block.id;
        scopes.jump_into_Block(scope);
        
        const Node& Stmts = Block.sons[1];
        for(auto Stmt : Stmts.sons){
            proc_Stmt(Stmt.sons[0]);
        }
        
        scopes.jump_out_Block();
//        variables.print_variables();
        variables.remove_scope(scope);
    }
    
    void proc_MAIN(const Node& MAIN){
        instructions.add_instruction(LABEL, NONE, NONE, "main");
        proc_Block(MAIN.sons[4]);
    }
    
    void proc_Stmt(const Node& Stmt){
        string stmt = Stmt.Component;
        int symbol = SYMBOL_MAP[Stmt.Component];
        
        
        switch (symbol) {
            case __Asig_E__:
                proc_Asig_E(Stmt);
                break;
            case __Decl__:
                proc_Decl(Stmt);
                break;
            case __IF_Block__:
                proc_IF_Block(Stmt);
                break;
                
            default:
                break;
        }
    }
    
    void proc_IF_Block(const Node& IF_Block){
        for(auto i : IF_Block.sons){
            int symbol = SYMBOL_MAP[i.Component];
            
            
            switch (symbol) {
                case __IF__:
                    proc_IF(i);
                    break;
                case __ELSE_IF__:
                    proc_ELSE_IF(i);
                    break;
                case __ELSE__:
                    proc_ELSE(i);
                    break;
                    
                default:
                    break;
            }
            
            
        }
    }
    
    int if_pos;
    string label_if_exit;
    
    void proc_IF(const Node& IF){
        get_Unit(IF.sons[2]);
        string label = Instruction::seriablize("IF", IF.id);
        label_if_exit = Instruction::seriablize("EXIT", IF.id);
        
        instructions.add_label(label);
        if_pos = instructions.get_pos();
        instructions.add_instruction(LABEL, NONE, NONE, label);
        proc_Block(IF.sons[4]);
        instructions.add_instruction(GOTO, NONE, NONE, label_if_exit);
    }
    
    void proc_ELSE_IF(const Node& ELSE_IF){
        proc_Block(ELSE_IF.sons[4]);
    }
    
    void proc_ELSE(const Node& ELSE){
        string label_else = Instruction::seriablize("ELSE", ELSE.id);

        instructions.add_instruction(GOTO, NONE, NONE, label_else, if_pos);
        
        instructions.add_instruction(LABEL, NONE, NONE, label_else);
        proc_Block(ELSE.sons[1]);
        instructions.add_instruction(LABEL, NONE, NONE, label_if_exit);
    }
    
    void proc_HEADER(const Node& HEADER){
        const Node& H_Stmts = HEADER.sons[0];
        for(auto H_Stmt : H_Stmts.sons){
            proc_H_Stmt(H_Stmt.sons[0]);
        }
    }
    
    void proc_Asig_E(const Node& Asig_E){
        
        string variable_name = Asig_E.sons[0].sons[0].Component;
        
        // seek this variable through the current usable scopes and its name
        int scope = variables.seek_variable(scopes.get_cur_usable_scopes(), variable_name);
        
        if(scope){
            string t = get_Unit(Asig_E.sons[2]);
            instructions.add_instruction(ASSIGN, t, NONE, Instruction::seriablize(variable_name, scope));
            
        }
    }
    
    string get_Unit(const Node& node){
        int unit = SYMBOL_MAP[node.Component];
        switch (unit) {
            case __Factor__:{
                return proc_Factor(node);
            }
            case __Expr__:{
                return proc_Expr(node);
            }
        }
        return NONE;
        
    }
    
    string proc_Expr(const Node& Expr){
        string num1 = get_Unit(Expr.sons[0]), num2 = get_Unit(Expr.sons[2]);
        string op = Expr.sons[1].Component;
        string res = instructions.get_temp();
        instructions.add_instruction(SYMBOL_MAP[op], num1, num2, res);
        return res;
    }
    

    string proc_Factor(const Node& Factor){
        
        int symbol = SYMBOL_MAP[Factor.sons[0].Component];
        
        
        switch (symbol) {
            // Integer Number : 0, 1, 99 ...
            case __INT__:{
                string val = Factor.sons[0].sons[0].Component;
                string res = instructions.get_temp();
                instructions.add_instruction(LOAD_IMMEDIATE, val, NONE, res);
                return res;
            }
            
            // Beginning as Identifier
            case __Id__:{
                string variable_name = Factor.sons[0].sons[0].Component;
                int scope = variables.seek_variable(scopes.get_cur_usable_scopes(), variable_name);
                if(scope){
                    string res = instructions.get_temp();
                    instructions.add_instruction(LOAD_VARIABLE, Instruction::seriablize(variable_name, scope), NONE, res);
                    return res;
                    
                }
                break;
            }
                
            case __LEFT_Bracket__:{
                return get_Unit(Factor.sons[1]);
            }
        }
        return NONE;
    }
    

    
    void proc_Decl(const Node& Decl){
        int type = TYPE_MAP[Decl.sons[0].sons[0].Component];
        string variable_name;
        
        for(auto Desc : Decl.sons[1].sons){
            if(Desc.Component == _Desc_){

                int desc_type = SYMBOL_MAP[Desc.sons[0].Component];
                
                
                switch (desc_type) {
                    case __Id__:
                        // Desc - Id - t
                        variable_name = Desc.sons[0].sons[0].Component;
                        variables.add_variable(scopes.get_cur_scope(), variable_name, type);
                        
                        instructions.add_instruction(ASSIGN, "0", NONE, Instruction::seriablize(variable_name, scopes.get_cur_scope()));
                        
                        break;
                        
                    case __Asig_E__:
                        // Desc - Asig_E - Id - t
                        variable_name = Desc.sons[0].sons[0].sons[0].Component;
                        if(variables.add_variable(scopes.get_cur_scope(), variable_name, type)){
                            proc_Asig_E(Desc.sons[0]);
                        }
                        break;
                        
                    default:
                        break;
                }
                
                
            }
        }
    }
    
    void proc_FunDef(const Node& FunDef){
        int return_type = TYPE_MAP[FunDef.sons[0].sons[0].Component];
        string function_name = FunDef.sons[1].sons[0].Component;
        
        vector<Parameter> parameters;
        for(auto ParaDef : FunDef.sons[3].sons){
            if(ParaDef.Component == _Para_Def_){
                parameters.push_back(make_pair(TYPE_MAP[ParaDef.sons[0].sons[0].Component], ParaDef.sons[1].sons[0].Component));
            }
        }
        
        functions.add_function(function_name, parameters, return_type);
    }
    
    void proc_H_Stmt(const Node& H_Stmt){
        int symbol = SYMBOL_MAP[H_Stmt.Component];
        
        switch (symbol) {
            case __Decl__:{
                proc_Decl(H_Stmt);
                break;
            }
                
            case __Fun_Def__:{
                proc_FunDef(H_Stmt);
                break;
            }
            default:
                break;
            
        }
    }
    
};


#endif /* SemanticAnalyzer_h */
