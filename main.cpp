#include <iostream>
#include <deque>
#include <string>
#include <sstream>
#include <vector>

using namespace std;

class PCB{
private:
    int state;
    int parent;
    int priority;
    deque<int> children;
    deque<int> resources;
public:
    //1 means ready
    //0 means block
    //-1 means new(not yet created)
    PCB(){
        this->state = -1;
        this->parent = -1;//if it is 
        this->priority= -1;
    };
    void setState(int state){
        this->state = state;
    }
    void setParent(int parent){
        this->parent = parent;
    }
    void setPriority(int priority){
        this->priority=priority;
    }
    int getPriority(){
        return this->priority;
    }
    void reset(){
        this->state = -1;
        this->parent = -1;
        this->priority= -1;
        this->children.clear();
        this->resources.clear();
    }
    //add child to children list
    void addChild(int child){
        this->children.push_back(child);
    }
    //receive resource from which RCB
    void addResource(int rcb){
        this->resources.push_back(rcb);
    }
    void rmResource(int rs){
        for(int i=0;i<resources.size();i++){
            if(resources[i]==rs){
                resources.erase(resources.begin()+i);
                break;
            }
        }
    }
};
struct PR{
    int process;
    int resource;
    PR(int p, int r){
        process=p;
        resource=r;
    }
};
class RCB{
private:
    int state;
    int freeInventory;
    int UsingInventory;
public:
    //which process is holding how many recourses
    deque<PR> resources;
    //which process is waiting for how many resources
    deque<PR> waitlist;
    RCB(){
        //1 free
        //0 allocated
        this->state = 1;
        this->freeInventory = 0;
        this->UsingInventory = 0;
    };
    int getState(){
        return this->state;
    }
    void setState(int state){
        this->state=state;
    }
    void setFreeInventory(int inventory){
        this->freeInventory = inventory;
    }
    void setUsingInventory(int inventory){
        this->UsingInventory = inventory;
    }
    void getResource(PR pr){
        //this process can get resource
        this->freeInventory -= pr.resource;
        this->UsingInventory += pr.resource;
        this->resources.push_back(pr);
        if(this->freeInventory==0){
            this->state = 0;
        }
    }
    //if no error, it can request
    bool request(int process,int units){
        PR pr(process, units);
        if(canRequest(units)){
            getResource(pr);
            return true;
        }
        addWaitlist(pr);  
        return false;
    }
    void release(int process, int releasingunits){
        int index = 0;
        for(int i=0;i<resources.size();i++){
            if(this->resources[i].process == process){
                index = i;
                break;
            }
        }
        int holdingunits = resources[index].resource;
        //check to release the whole or some resources
        if(holdingunits==releasingunits){
            this->resources.erase(resources.begin()+index);
        }else{
            this->resources[index].resource-=releasingunits;
        }
        freeInventory+=releasingunits;
        UsingInventory-=releasingunits;
        //state check
        if(this->state==0){
            this->state=1;
        }
    }
    bool canRequest(int request){
        return this->freeInventory>=request;
    }
    int getFreeInventory(){
        return this->freeInventory;
    }
    int getUsingInventory(){
        return this->UsingInventory;
    }
    //erase the data
    void reset(){
        this->state = 1;
        this->freeInventory = 0;
        this->UsingInventory = 0;
        this->waitlist.clear();
    }
    void addWaitlist(PR pr){
        this->waitlist.push_back(pr);
    }
    int getWaitlistSize(){
        return this->waitlist.size();
    }
};

void getCommands(vector<string>& thisArgs, string str)
{
    string word;
    // making a string stream
    stringstream iss(str);
    //store words to thisArgs and return that
    while (iss >> word){
        thisArgs.push_back(word);
    }
        
}

class Manager{
private:  
    PCB pcb[16];
    RCB rcb[4];
    deque<int>* RL;
    //current runningProc process and its level
    int runningProc;
    int runningLevel;
    //index of process that is not yet created
    int indexProcess;
    //the total level of RL
    int totalLevel;
public:
    Manager(){};
    ~Manager(){
        delete[] RL;
    }
    int getTotalLevel(){
        return this->totalLevel;
    }
    //when create a new process, it needs to increase the index
    void increIndexProc(){
        this->indexProcess++;
    }
    void scheduler(){
        //find highest priority ready process j
        //j: head of highest‐priority non‐empty list (RL)
        //real scheduler may perform context switch
        //implicit in our case:
        //if currently runningProc process is still the head of the highest‐priority list: 
        //no context switch
        //if any function has changed the head of that list: context switch
        int priProcess = 0;
        int levelrunning = 0;
        for(int i=1;i<totalLevel;i++){
            if(RL[i].size()>0){
                priProcess = RL[i].front();
                levelrunning = i;
                break;
            }
        }
        if(runningProc!=priProcess){
            this->runningProc = priProcess;
            this->runningLevel = levelrunning;
        }
        //display: “process j runningProc”
        cout<<this->runningProc<<" ";
    }
    void create(int level){
        //allocate new PCB[j]
        //state = ready
        pcb[this->indexProcess].setState(1);
        //insert j into list of children of i(runningProc)
        pcb[this->runningProc].addChild(indexProcess);
        //parent = i
        pcb[this->indexProcess].setParent(this->runningProc);
        //children = NULL resources = NULL (no need to take care of this)
        //insert j into RL
        RL[level].push_back(this->indexProcess);
        //display: “process j created”
        //cout<< this->indexProcess <<endl;
        increIndexProc();
        scheduler();
    }
    void destroy();
    void request(int r, int units){
        if(rcb[r].getFreeInventory()+rcb[r].getUsingInventory()< units){
            cout<<"-1 ";
            return;
        }
        if(!rcb[r].request(this->runningProc, units)){
            //If TRUE, it request successfull
            //If not, the process is blocked
            pcb[this->runningProc].setState(0);
            RL[this->runningLevel].pop_front();
            scheduler();
        }else{
            cout<<this->runningProc<<" ";
        }
    }
    void release(int r, int releasingunits){
        
        int index = 0;
        for(int i=0;i<rcb[r].resources.size();i++){
            if(this->rcb[r].resources[i].process == runningProc){
                index = i;
                break;
            }
        }
        int holdingunits = rcb[r].resources[index].resource;
        if(releasingunits>holdingunits){
            cout<<"-1 ";
            return;
        }
        //check to release the whole or some resources
        if(holdingunits==releasingunits){
            //remove (r, k) from rcb[r].resources
            this->rcb[r].resources.erase(rcb[r].resources.begin()+index);
            //remove resource from resourse list in running process
            pcb[runningProc].rmResource(r);
        }else{
            this->rcb[r].resources[index].resource-=releasingunits;
        }
        //r.free = r.free + k
        rcb[r].setFreeInventory(rcb[r].getFreeInventory()+releasingunits);
        rcb[r].setUsingInventory(rcb[r].getUsingInventory()-releasingunits);
        //state check
        if(rcb[r].getState()==0){
            rcb[r].setState(1);
        }
        bool canRequest = true;
        int pr;
        int rs;
            //while (r.waitlist != empty and r.free > 0)
        while(rcb[r].waitlist.size()>0 && canRequest){
            //get next (j, k) from r.waitlist
            pr = rcb[r].waitlist.front().process;
            rs = rcb[r].waitlist.front().resource;
            //if there are enough resource for waiting process if (r.free >= k)
            if(rs<=rcb[r].getFreeInventory()){
                //r.free = r.free - k
                rcb[r].getResource(PR(pr,rs));
                //insert r into j.resources
                pcb[pr].addResource(r);
                //j.state = ready
                pcb[pr].setState(1);
                //remove (j, k) from r.waitlist
                rcb[r].waitlist.pop_front();
                //insert j into RL
                int level = pcb[pr].getPriority();
                RL[level].push_back(pr);
            }else{
                canRequest=false;
            }
        }
        scheduler();
    }
    
    void timeout(){
        //move process i from head of RL to end of RL of that level
        if(RL[this->runningLevel].size()==1) {
            cout<<this->runningProc<<" ";
            return;
        }
        int process = RL[this->runningLevel].front();
        RL[this->runningLevel].pop_front();
        RL[this->runningLevel].push_back(process);
        scheduler();
    }
    //reset all data to init
    void reset(){
        for(int i=0;i<16;i++){
            pcb[i].reset();
        }
        for(int i=0;i<4;i++){
            rcb[i].reset();
        }
        delete[] RL;
    }
    void init(bool first, int level, int r1, int r2, int r3, int r4){
        //erase all previous data
        if(!first) reset();
        //set value
        pcb[0].setPriority(0);
        rcb[0].setFreeInventory(r1);
        rcb[1].setFreeInventory(r2);
        rcb[2].setFreeInventory(r3);
        rcb[3].setFreeInventory(r4);
        RL = new deque<int>[level];
        RL[0].push_back(0);
        runningProc = 0;
        runningLevel = 0;
        indexProcess = 1;
        this->totalLevel = level;
        //print the current runningProc process
        if(first) cout<<"0 ";
        else cout<<endl<<"0 ";
    };
    void init_default(bool first){
        init(first, 3, 1, 1, 2, 3);
    }
};

int main(){
    string command;
    vector<string> argms;
    //get the command from users
    bool isStopped = false;
    Manager manager;
    int first = true;
    while(!isStopped){
        cout<< "> ";
        getline(cin, command);
        //get each word from command
        getCommands(argms, command);
        int argmsize = argms.size();
        //cout<<"argument size:" <<argmsize<<endl;
        if(argmsize<=6){
            if(argmsize == 4 || argmsize == 5) {
                argms.clear();
                cout<<"-1"; //incorrect command
                continue;
            }
            if(argms[0]=="in" && argmsize==6){
                //stoi turn string to int
                manager.init(first, stoi(argms[1]),stoi(argms[2]),stoi(argms[3]),stoi(argms[4]),stoi(argms[5]));
                first = false;
            }else if(argms[0]=="id" && argmsize==1){
                manager.init_default(first);
                first = false;
            }else if(argms[0]=="cr" && argmsize==2){
                int level = stoi(argms[1]);
                if(level<=0 || level>=manager.getTotalLevel()){
                    cout<<"-1"; //incorrect priority
                    continue;
                }
                manager.create(level);
            }else if(argms[0]=="to" && argmsize==1){
                manager.timeout();
            }else if(argms[0]=="rq" && argmsize==3){
                int r = stoi(argms[1]);
                int k = stoi(argms[2]);
                if(r<0 || r>manager.getTotalLevel()){
                    cout<<"-1 ";
                    continue;
                }
                manager.request(r,k);
            }else if(argms[0]=="rl" && argmsize==3){
                int r = stoi(argms[1]);
                int k = stoi(argms[2]);
                if(r<0 || r>manager.getTotalLevel()){
                    cout<<"-1 ";
                    continue;
                }
                manager.release(r,k);
            }
            else if(argms[0]=="stop"){
                isStopped=true;
            }
        }else{
            argms.clear();
            cout<<"-1";//Invalid command
            continue;
        }
        argms.clear();
    }
    return 0;
}