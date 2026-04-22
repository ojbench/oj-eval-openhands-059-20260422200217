#include <bits/stdc++.h>
#include "src.hpp"
using namespace std;
string get_data () {
    string ret = "" , line;
    while (getline(cin , line)) {
        if (line.length() == 0) continue;
        if (line == "#####") break;
        ret += line + "\n";
    }
    return ret;
}
int main(){
    char judger_name[100];
    if(!(cin>>judger_name)) return 0;
    size_t time_limit, mem_limit; cin>>time_limit>>mem_limit; 
    BaseJudger* judger=nullptr; string tmp; 
    if(strcmp(judger_name,"OIJudger")==0){ tmp=get_data(); judger=new OIJudger(time_limit,mem_limit,tmp.c_str()); }
    else if(strcmp(judger_name,"ICPCJudger")==0){ tmp=get_data(); judger=new ICPCJudger(time_limit,mem_limit,tmp.c_str()); }
    else if(strcmp(judger_name,"SpacialJudger")==0){ size_t fst,fsm; cin>>fst>>fsm; tmp=get_data(); judger=new SpacialJudger(time_limit,mem_limit,fst,fsm,tmp.c_str()); }
    size_t t,m; while(cin>>t>>m){ tmp=get_data(); judger->Submit(t,m,tmp.c_str()); }
    cout<<judger->GetScore()<<"\n"; delete judger; return 0;
}
