//
//  ScopeManager.h
//  CompilerProject3
//
//  Created by PatrickDD on 2021/5/17.
//

#ifndef ScopeManager_h
#define ScopeManager_h


class ScopeManager{
private:
    vector<int> usable_scopes;

public:
    ScopeManager(){}
    
    const vector<int>& get_cur_usable_scopes(){
        return usable_scopes;
    }
    
    int get_cur_scope(){
        return usable_scopes.back();
    }
    
    int get_HEADER_scope(){
        return usable_scopes.front();
    }
    
    void jump_into_Block(int Block_id){
        usable_scopes.push_back(Block_id);
    }
    
    void jump_out_Block(){
        usable_scopes.pop_back();
    }
};

#endif /* ScopeManager_h */
