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
    //-1 means running
    PCB(){
        this->state = 1;
        this->parent = -1;//if it is 
        this->priority= -1;
    };
    void setParent(int parent){
        this->parent = parent;
    }
    void setPriority(int priority){
        this->priority=priority;
    }
    void reset(){
        this->state = 1;
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
    Manager(){};
    ~Manager(){
        delete[] RL;
    }
    void create();
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
    void init(int level, int r1, int r2, int r3){
        pcb[0].setPriority(0);
        rcb[0].setInventory(r1);
        rcb[1].setInventory(r2);
        rcb[2].setInventory(r3);
        RL = new deque<int>[level];
        RL->push_back(0);
    };
    void init_default();
};

int main(){
    string command;
    vector<string> argms;
    //get the command from users
    bool isStopped = false;
    while(!isStopped){
        cout<< '> '<< endl;
        getline(cin, command);
        //get each word from command
        getCommands(argms, command);
        int argmsize = argms.size();
        if(argmsize<=6){
            if(argmsize == 4 || argmsize == 5) {
                argms.clear();
                cout<<"Invalid command\n";
                continue;
            }
            if(argms[0]=="in" && argmsize==6){
            }

        }else{
            argms.clear();
            cout<<"Invalid command\n";
            continue;
        }
    }
    

}