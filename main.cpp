// argv[1]:   input.txt
#include "Def.h"
using namespace std;

int main(int argc, char* argv[]){
    vector<baseStation> BS_list;
    vector<action> action_list;
    
    // for QL
    double cell_radius = 250;
    const int ECB_size = 4;
    double ECB[ECB_size] = {0.4, 0.52, 0.68, 0.76};
    vector<int> BSaction;
    
    // Read Input //
    if(readInputOpt(argv[1],BS_list)==false)
        return 0;

    // QL action init //
    QLinit(cell_radius, ECB, ECB_size, action_list);
    
    // QL state init //
    for(int i=0;i<BS_list.size();i++){
        QLConfigBSUE(BS_list, action_list, i, 0);
        BSaction.push_back(0);
    }
    // RB alloc //
    RBalloc(BS_list);
    
    // Calc Sub-band SINR of all UEs //
    calcsubSINR(BS_list);
    
    // Calc avg. SINR //
    calcavgSINR(BS_list);
    
    double maxSINR=calcAllSINR(BS_list);
    
    // Single BS RB Re-init and RBalloc //
    bool isTerminated=0;
    double maxSINRinFour=maxSINR;       // four adj action
    double maxSINRinFourActionIdx=-1;
    while(!isTerminated){
        for(int i=0;i<BS_list.size();i++){
            vector<int> vaild_action;
            if(BSaction[i]+1<action_list.size())
                vaild_action.push_back(BSaction[i]+1);
            if(BSaction[i]+7<action_list.size())
                vaild_action.push_back(BSaction[i]+7);
            if(BSaction[i]-1>-1)
                vaild_action.push_back(BSaction[i]-1);
            if(BSaction[i]-7>-1)
                vaild_action.push_back(BSaction[i]-7);
            
            if(vaild_action.size()==0)
                continue;
            
            
            //RBallocSingleBS(BS_list, i);
        }
    }
    // Select UE CQI by SINR //
    for(int i=0;i<BS_list.size();i++)
        for(int j=0;j<BS_list[i].UE_list.size();j++)
            BS_list[i].UE_list[j].CQI=selectCQI(BS_list[i].UE_list[j].avgSINR);
    
    // Select UE MCS by SINR //
    for(int i=0;i<BS_list.size();i++)
        for(int j=0;j<BS_list[i].UE_list.size();j++)
            BS_list[i].UE_list[j].MCS=selectMCS(BS_list[i].UE_list[j].avgSINR);
    
    //showUEsinr(BS_list);
    showUEinfo(BS_list);
    showUEallocRB(BS_list);
    showBSinfo(BS_list);
    
    return 0;
}