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
    public:
    PCB pcb[16];
    RCB rcb[4];
    deque<int>* RL;
    //current running process
    int running;
    //index of process that is not yet created
    int indexProcess;
    Manager(){};
    ~Manager(){
        delete[] RL;
    }
    void create(int j){
        //allocate new PCB[j]
        //state = ready
        pcb[j].setState(1);
        //insert j into list of children of i
        
        //parent = i
        //children = NULL 
        //resources = NULL 
        //insert j into RL
        //display: “process j created”
    }
    void destroy();
    void request();
    void release();
    void timeout();
    void scheduler();
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
        //print the current running process
        cout<<"0"<<endl;
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
                cout<<"Invalid command\n";
                continue;
            }
            if(argms[0]=="in" && argmsize==6){
                //stoi turn string to int
                manager.init(first, stoi(argms[1]),stoi(argms[2]),stoi(argms[3]),stoi(argms[4]),stoi(argms[5]));
                first = false;
            }else if(argms[0]=="id" && argmsize==1){
                manager.init_default(first);
                first = false;
            }else if(argms[0]=="stop"){
                isStopped=true;
            }
        }else{
            argms.clear();
            cout<<"Invalid command\n";
            continue;
        }
        argms.clear();
    }
    return 0;
}