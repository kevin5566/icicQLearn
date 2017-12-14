// argv[1]:   input.txt
//** cell_radius should modify to correct value **// 
#include "Def.h"
using namespace std;

int main(int argc, char* argv[]){
    vector<baseStation> BS_list;
    vector<action> action_list;
    
    // for QL
    double cell_radius = 250; 
    const int ECB_size = 4;
    double ECB[ECB_size] = {0.4, 0.52, 0.68, 0.76}; //10/25, 13/25, 17/25, 19/25
    vector<int> BSaction;       // Record now BS action
    vector<bool> isBSstable;    // BS Need learning or not
    
    // Read Input //
    if(readInputOpt(argv[1],BS_list)==false)
        return 0;

    // QL action init //
    QLinit(cell_radius, ECB, ECB_size, action_list);
    
    // QL state init //
    for(int i=0;i<BS_list.size();i++){
        for(int j=0;j<N_band;j++){
            if(BS_list[i].RB_pa[j]!=7){
                BSaction.push_back(BS_list[i].RB_pa[j]);   // Record now BS action
                break;
            }
        }
        isBSstable.push_back(0);    // All need learning now, 0
    }
    
    // 3 pre-step for calcAllSINR //
    RBalloc(BS_list);       // RB alloc
    calcsubSINR(BS_list);   // Calc Sub-band SINR of all UEs
    calcavgSINR(BS_list);   // Calc avg. SINR per UE
    
    // Calc avg. of All UE SINR //
    double maxSINR=calcAllSINR(BS_list);
    
    // Single BS RB Re-init and RBalloc //
    bool isTerminated=0;
    
    double maxSINRinFour=maxSINR;       // four adj action
    double maxSINRinFourActionIdx=-1;   // next step idx, -1 stand for no change
    double tmpSINR=0;
    int round_of_learn=100;
    
    while(!isTerminated){   // Terminate learning when all BS stable
        for(int i=0;i<BS_list.size();i++){  // i: idx of BS
            // Queueing possible action of now BS
            vector<int> vaild_action;
            if(BSaction[i]+1<action_list.size())
                vaild_action.push_back(BSaction[i]+1);
            if(BSaction[i]+7<action_list.size())
                vaild_action.push_back(BSaction[i]+7);
            if(BSaction[i]-1>-1)
                vaild_action.push_back(BSaction[i]-1);
            if(BSaction[i]-7>-1)
                vaild_action.push_back(BSaction[i]-7);
            
            maxSINRinFour=maxSINR;
            maxSINRinFourActionIdx=-1;
            for(int j=0;j<vaild_action.size();j++){
                initBSSingle(BS_list, i);
                QLConfigBSUE(BS_list, action_list, i, vaild_action[j]);
                RBallocSingleBS(BS_list, i);
                calcsubSINR(BS_list);
                calcavgSINR(BS_list);
                tmpSINR=calcAllSINR(BS_list);
                if(tmpSINR>maxSINRinFour){
                    maxSINRinFour=tmpSINR;
                    maxSINRinFourActionIdx=vaild_action[j];
                }
            }
            
            if(maxSINRinFourActionIdx!=-1){
                isBSstable[i]=0;
                maxSINR=maxSINRinFour;
                BSaction[i]=maxSINRinFourActionIdx;
                initBSSingle(BS_list, i);
                QLConfigBSUE(BS_list, action_list, i, BSaction[i]);
                RBallocSingleBS(BS_list, i);
            }
            else{
                isBSstable[i]=1;
                initBSSingle(BS_list, i);
                QLConfigBSUE(BS_list, action_list, i, BSaction[i]);
                RBallocSingleBS(BS_list, i);
            }
        }
        --round_of_learn;
        cout<<round_of_learn<<endl;
        //showBSinfo(BS_list);
        for(int i=0;i<BSaction.size();i++)
            cout<<BSaction[i]<<" ";
        cout<<endl;
        cout<<round_of_learn<<endl;
        if(round_of_learn==0)
            isTerminated=1;
        if(accumulate(isBSstable.begin(), isBSstable.end(), 0)==isBSstable.size())
            isTerminated=1;
    }
    
    calcsubSINR(BS_list);
    calcavgSINR(BS_list);
    
    // Select UE CQI by SINR //
    for(int i=0;i<BS_list.size();i++)
        for(int j=0;j<BS_list[i].UE_list.size();j++)
            BS_list[i].UE_list[j].CQI=selectCQI(BS_list[i].UE_list[j].avgSINR);
    
    // Select UE MCS by SINR //
    for(int i=0;i<BS_list.size();i++)
        for(int j=0;j<BS_list[i].UE_list.size();j++)
            BS_list[i].UE_list[j].MCS=selectMCS(BS_list[i].UE_list[j].avgSINR);
    
    //showUEsinr(BS_list);
    //showUEinfo(BS_list);
    //showUEallocRB(BS_list);
    //showBSinfo(BS_list);

	cout<<round_of_learn<<endl;
    
    // action_list
    // idx  pa   ECB
    // 0~6  0~6  0
    // +7   0~6  1
    // +7   0~6  2
    // +7   0~6  3

    return 0;
}
