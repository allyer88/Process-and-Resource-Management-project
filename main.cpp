#include <iostream>
#include <deque>
#include <string>
#include <sstream>
#include <vector>
#include <fstream>
//pcb state
#define READY 1
#define BLOCKED 0
#define NEW -1
//rcb state
#define FREE 1
#define AllOCATED 0
using namespace std;
struct RR{
    int rcb;
    int unit;
    RR(int r, int u){
        rcb=r;
        unit = u;
    }
};
class PCB{
private:
    int state;
    int parent;
    int priority;
    int blockedRCB;
public:
    deque<int> children;
    deque<RR> resources;
    //1 means ready
    //0 means block
    //-1 means free(not yet created)
    void printInfo(){
        cout<<"state: "<<state<<endl;
        cout<<"parent: "<<parent<<endl;
        cout<<"priority: "<<priority<<endl;
        cout<<"blocked RCB: "<<blockedRCB<<endl;
        cout<<"children: ";
        for(int i=0;i<children.size();i++){
            cout<<children[i]<<" ";
        }
        cout<<endl;
        cout<<"resources: ";
        for(int i=0;i<resources.size();i++){
            cout<<"("<<resources[i].rcb<<", "<< resources[i].unit<<")\n";
        }
        cout<<endl;
    }
    PCB(){
        this->state = NEW;
        this->parent = -1;//if it is 
        this->priority= -1;
        this->blockedRCB = -1;
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
    void setBlockedRCB(int rcb){
        this->blockedRCB=rcb;
    }
    int getBlockedRCB(){
        return this->blockedRCB;
    }
    int getPriority(){
        return this->priority;
    }
    int getState(){
        return this->state;
    }
    int getParent(){
        return this->parent;
    }
    void reset(){
        this->state = NEW;
        this->parent = -1;
        this->priority= -1;
        this->blockedRCB=-1;
        this->children.clear();
        this->resources.clear();
    }
    //add child to children list
    void addChild(int child){
        this->children.push_back(child);
    }
    //receive resource from which RCB
    void addResource(RR rcb){
        this->resources.push_back(rcb);
    }
    void rmResource(int rs){
        for(int i=0;i<resources.size();i++){
            if(resources[i].rcb==rs){
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
        this->state = FREE;
        this->freeInventory = 0;
        this->UsingInventory = 0;
    };
    void printInfo(){
        cout<<"state: "<<state<<endl;
        cout<<"How many free resources: "<<freeInventory<<endl;
        cout<<"How many allocated resources: "<<UsingInventory<<endl;
        cout<<"wailtlist: ";
        for(int i=0;i<waitlist.size();i++){
            cout<<"("<<waitlist[i].process<<", "<< waitlist[i].resource<<")\n";
        }
        cout<<endl;
        cout<<"PCB using resources: ";
        for(int i=0;i<resources.size();i++){
            cout<<"("<<resources[i].process<<", "<< resources[i].resource<<")\n";
        }
        cout<<endl;
    }
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
            this->state = AllOCATED;
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
        if(this->state==AllOCATED){
            this->state=FREE;
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
        this->state = FREE;
        this->freeInventory = 0;
        this->UsingInventory = 0;
        this->waitlist.clear();
        this->resources.clear();
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
    void printinfo(){
        cout<<"Current running process is: "<<runningProc<<endl;
        for(int i=0;i<indexProcess;i++){
            cout<<"PCB "<<i<<": "<<endl;
            pcb[i].printInfo();
        }
        cout<<endl;
        for(int i=0;i<4;i++){
            cout<<"RCB "<<i<<": "<<endl;
            rcb[i].printInfo();
        }
        cout<<endl;
        for(int i=0;i<totalLevel;i++){
            cout<<"RL level "<<i<<": ";
            for(int j=0;j<RL[i].size();j++){
                cout<<RL[i][j]<<" ";
            }
            cout<<endl;
        }
    }
    //when create a new process, it needs to increase the index
    void increIndexProc(){
        this->indexProcess++;
    }
    void scheduler(bool printProcess){
        //find highest priority ready process j
        //j: head of highest‐priority non‐empty list (RL)
        //real scheduler may perform context switch
        //implicit in our case:
        //if currently runningProc process is still the head of the highest‐priority list: 
        //no context switch
        //if any function has changed the head of that list: context switch
        int priProcess = 0;
        int levelrunning = 0;
        for(int i=totalLevel-1;i>=0;i--){
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
        if(printProcess){
            cout<<this->runningProc<<" ";
        }
        
    }
    void create(int level){
        //Error: Creating more than n processes
        if(indexProcess==16){
            cout<<"-1 ";
            return; 
        }
        //allocate new PCB[j]
        //state = ready
        pcb[this->indexProcess].setState(READY);
        //set its priority
        pcb[this->indexProcess].setPriority(level);
        //parent = i
        pcb[this->indexProcess].setParent(this->runningProc);
        //insert j into list of children of i(runningProc)
        pcb[this->runningProc].addChild(indexProcess);
        //children = NULL resources = NULL (no need to take care of this)
        //insert j into RL
        RL[level].push_back(this->indexProcess);
        //display: “process j created”
        //cout<< this->indexProcess <<endl;
        increIndexProc();
        scheduler(true);
        //printinfo();
    }
    void rmFromRL(int process){
        int level=pcb[process].getPriority();
        int size = RL[level].size();
        for(int i=0;i<size;i++){
            if(RL[level][i]==process){
                RL[level].erase(RL[level].begin()+i);
                break;
            }
        }
    }

    //when delete from the waitlist, next waiting process might able to use the resources
    void checkNextWLAvailable(int r){
        while(rcb[r].getFreeInventory()>0){
            int requestUnit = rcb[r].waitlist.front().resource;
            int process = rcb[r].waitlist.front().process;
            if(rcb[r].getFreeInventory()>=requestUnit){
                rcb[r].getResource(rcb[r].waitlist.front());
                pcb[process].addResource(RR(r, requestUnit));
                pcb[process].setBlockedRCB(-1);//not block
                pcb[process].setState(READY);
                int level = pcb[process].getPriority();
                RL[level].push_back(process);
            }
        }
    }
    void rmFromWl(int r, int process){
        int size = rcb[r].waitlist.size();
        for(int i=0;i<size;i++){
            if(rcb[r].waitlist[i].process==process){
                rcb[r].waitlist.erase(rcb[r].waitlist.begin()+i);
                //if the process is the front of waitlist, next process might able to get the resources
                if(i==0){
                    checkNextWLAvailable(r);
                }
                break;
            }
        }
        
    }
    //check whether the process that user wants to destroy
    //is current running process or one of its child
    bool hasDestroyError(int process){
        //Error: cannot destroy process 0
        if(process==0) return true;
        if(process==runningProc) return false;
        for(int i=0;i<pcb[runningProc].children.size();i++){
            if(pcb[runningProc].children[i]==process) return false;
        }
        return true;
    }
    void recurDestroy(int process){
        //destroy its child
        while(pcb[process].children.size()>0){
            int child = pcb[process].children.front(); 
            //cout<< "destroy "<< process << "'s child "<< child<<endl;
            recurDestroy(child);
        }
        //release all resources of j
        while(pcb[process].resources.size()>0){
            int r = pcb[process].resources.front().rcb;
            int unit = pcb[process].resources.front().unit;
            release(false, r, unit, process);
            //pcb[process].resources.pop_front();
        }
        //remove j from RL or waiting list
        //if ready then in RL
        //else is blocked in some resource's waiting list 
        if(pcb[process].getState()==READY){
            rmFromRL(process);
        }else{
            int blockedRCB = pcb[process].getBlockedRCB();
            rmFromWl(blockedRCB, process);
        }
        //remove j from parent’s list of children 
        int parent = pcb[process].getParent();
        pcb[parent].children.pop_front();
        //free PCB of j
        pcb[process].reset();
        //printinfo();
    }
    void destroy(int process){
        recurDestroy(process);
        scheduler(true);
    }
    void request(int r, int units){
        //Error: Process 0 should be prevented from requesting any resource
        if(runningProc==0||(rcb[r].getFreeInventory()+rcb[r].getUsingInventory()< units)){
            cout<<"-1 ";
            return;
        }
        if(!rcb[r].request(this->runningProc, units)){
            //If TRUE, it request successfull
            //If not, the process is blocked
            pcb[this->runningProc].setState(BLOCKED);
            pcb[this->runningProc].setBlockedRCB(r);
            RL[this->runningLevel].pop_front();
            scheduler(true);
        }else{
            pcb[this->runningProc].addResource(RR(r, units));
            cout<<this->runningProc<<" ";
        }
        //printinfo();
    }
    void release(bool printProcess, int r, int releasingunits, int process){
        //if pass -1 mean it is from command rl so it is running process
        if(process==-1) process = runningProc;
        int index = -1;
        for(int i=0;i<rcb[r].resources.size();i++){
            if(this->rcb[r].resources[i].process == process){
                index = i;
                break;
            }
        }
        if(index==-1){
            //Error: Releasing a resource the process is not holding
            cout<<"-1 ";
            return;
        }
        int holdingunits = rcb[r].resources[index].resource;
        //Error: number of units released must ≤ number of units currently held
        if(releasingunits>holdingunits){
            cout<<"-1 ";
            return;
        }
        //check to release the whole or some resources
        if(holdingunits==releasingunits){
            //remove (r, k) from rcb[r].resources
            this->rcb[r].resources.erase(rcb[r].resources.begin()+index);
            //remove resource from resourse list in running process
            pcb[process].rmResource(r);
        }else{
            this->rcb[r].resources[index].resource-=releasingunits;
        }
        //r.free = r.free + k
        rcb[r].setFreeInventory(rcb[r].getFreeInventory()+releasingunits);
        rcb[r].setUsingInventory(rcb[r].getUsingInventory()-releasingunits);
        //state check
        if(rcb[r].getState()==AllOCATED){
            rcb[r].setState(FREE);
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
                pcb[pr].addResource(RR(r,rs));
                //j.state = ready
                pcb[pr].setState(READY);
                //remove block rcb data
                pcb[pr].setBlockedRCB(-1);
                //remove (j, k) from r.waitlist
                rcb[r].waitlist.pop_front();
                //insert j into RL
                int level = pcb[pr].getPriority();
                RL[level].push_back(pr);
            }else{
                canRequest=false;
            }
        }
        scheduler(printProcess);
        //printinfo();
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
        scheduler(true);
        //printinfo();
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
        pcb[0].setState(READY);
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
        //printinfo();
    };
    void init_default(bool first){
        init(first, 3, 1, 1, 2, 3);
    }
};

int main(int argc, char* argv[]){
    if(argc!=2){
        cout<<"Please enter './main input.txt' to run the program\n";
        return 0;
    }
    string filename = argv[1];
    // Open the input file
    ifstream inputFile(filename);
    if (!inputFile) {
        cout << "Error: Unable to open the file." << endl;
        return 0;
    }
    string command;
    vector<string> argms;
    Manager manager;
    int first = true;
    while(getline(inputFile, command)){
        //if it is a newline
        if(command.empty()) continue;
        //get each word from command
        getCommands(argms, command);
        int argmsize = argms.size();
        //cout<<"argument size:" <<argmsize<<endl;
        if(argmsize<=6){
            if(argms[0]=="in" && argmsize==6){
                //stoi turn string to int
                manager.init(first, stoi(argms[1]),stoi(argms[2]),stoi(argms[3]),stoi(argms[4]),stoi(argms[5]));
                first = false;
            }else if(argms[0]=="id" && argmsize==1){
                manager.init_default(first);
                first = false;
            }else if(argms[0]=="cr" && argmsize==2){
                int level = stoi(argms[1]);
                //Error: 0 is reserved for process 0, so no level 1
                if(level<=0 || level>=manager.getTotalLevel()){
                    cout<<"-1 ";
                    argms.clear(); //incorrect priority
                    continue;
                }
                manager.create(level);
            }else if(argms[0]=="to" && argmsize==1){
                manager.timeout();
            }else if(argms[0]=="rq" && argmsize==3){
                int r = stoi(argms[1]);
                int k = stoi(argms[2]);
                //Error: Requesting a nonexistent resource
                if(r<0 || r>=4){
                    cout<<"-1 ";
                    argms.clear();
                    continue;
                }
                manager.request(r,k);
            }else if(argms[0]=="rl" && argmsize==3){
                int r = stoi(argms[1]);
                int k = stoi(argms[2]);
                if(r<0 || r>=4){
                    cout<<"-1 ";
                    argms.clear();
                    continue;
                }
                manager.release(true,r,k,-1);
            }else if(argms[0]=="de" && argmsize==2){ 
                //Error: Destroying a process that is not a child of the current process
                if(manager.hasDestroyError(stoi(argms[1]))){
                    cout<<"-1 ";
                    argms.clear();
                    continue;
                }
                manager.destroy(stoi(argms[1]));
            }else{
                argms.clear();
                //Error: invalid command
                cout<<"-1 "; 
                continue;
            }
        }else{
            argms.clear();
            //Error: invalid command
            cout<<"-1 ";
            continue;
        }
        argms.clear();
    }
    inputFile.close();
    return 0;
}