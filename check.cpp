#include <iostream>
#include <fstream>
using namespace std;
#include <algorithm> // for std::remove_if
#include <cctype>    // for std::isspace
// Function to remove spaces, tabs, and newline characters from a string
void removeSpacesAndNewlines(string& str) {
    str.erase(remove_if(str.begin(), str.end(), [](char c) { return isspace(c) || c == '\n' || c == '\t'; }), str.end());
}

// Function to trim leading and trailing spaces from a string
void trim(string& str) {
    size_t start = str.find_first_not_of(" \t\n");
    size_t end = str.find_last_not_of(" \t\n");
    if (start != string::npos && end != string::npos)
        str = str.substr(start, end - start + 1);
    else
        str.clear(); // String contains only whitespace
}
int main(){
    string myOutputFile = "output.txt";
    string answerFile = "answer.txt";
    ifstream myFile(myOutputFile);
    ifstream realFile(answerFile);
    string answer;
    string myanswer;
    while(getline(realFile, answer)){
        getline(myFile, myanswer);
        //removeSpacesAndNewlines(answer);
        //removeSpacesAndNewlines(myanswer);
        // Trim leading and trailing spaces from both strings
        //trim(answer);
        //trim(myanswer);
        cout<<"-----------------------"<<endl;
        cout<<answer<<endl;
        cout<<myanswer<<endl;
        cout<<"-----------------------"<<endl;
    }
    myFile.close();
    realFile.close();
}