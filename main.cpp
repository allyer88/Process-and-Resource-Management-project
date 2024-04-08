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
};

class RCB{
private:
    int state;
    int inventory;
    deque<int> waitlist;
public:
    RCB(){
        this->state = 1;
        this->inventory = 0;
    };
    void setInventory(int inventory){
        this->inventory = inventory;
    }
    //erase the data
    void reset(){
        this->state = 1;
        this->inventory = 0;
        this->waitlist.clear();
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
    //current running process
    int running;
    //index of process that is not yet created
    int indexProcess;
    //the level of RL
    int level;
public:
    Manager(){};
    ~Manager(){
        delete[] RL;
    }
    int getLevel(){
        return this->level;
    }
    //when create a new process, it needs to increase the index
    void increIndexProc(){
        this->indexProcess++;
    }
    void create(int level){
        //allocate new PCB[j]
        //state = ready
        pcb[this->indexProcess].setState(1);
        //insert j into list of children of i(running)
        pcb[this->running].addChild(indexProcess);
        //parent = i
        pcb[this->indexProcess].setParent(this->running);
        //children = NULL resources = NULL (no need to take care of this)
        //insert j into RL
        RL[level].push_back(this->indexProcess);
        //display: “process j created”
        //cout<< this->indexProcess <<endl;
        increIndexProc();
        scheduler();
    }
    void destroy();
    void request();
    void release();
    void timeout();
    void scheduler(){
        //find highest priority ready process j
        //j: head of highest‐priority non‐empty list (RL)
        //real scheduler may perform context switch
        //implicit in our case:
        //if currently running process is still the head of the highest‐priority list: 
        //no context switch
        //if any function has changed the head of that list: context switch
        for(int i=1;i<level;i++){
            if(RL[i].size()>0){
                this->running =  RL[i].front();
                break;
            }
        }
        //display: “process j running”
        cout<<this->running<<" ";
        
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
        rcb[0].setInventory(r1);
        rcb[1].setInventory(r2);
        rcb[2].setInventory(r3);
        rcb[3].setInventory(r4);
        RL = new deque<int>[level];
        RL[0].push_back(0);
        running = 0;
        indexProcess = 1;
        this->level = level;
        //print the current running process
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
                if(level<=0 || level>=manager.getLevel()){
                    cout<<"-1"; //incorrect priority
                    continue;
                }
                manager.create(level);
            }else if(argms[0]=="stop"){
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