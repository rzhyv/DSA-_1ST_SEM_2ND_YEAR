#include <iostream>
#include <iomanip>
#include <string>
#include <fstream> //file stream - creates, reads, and writes to files.
#include <sstream> //string stream - to read and write to strings like they are streams.
#define MAX 5

using namespace std;

const string file = "Record.csv";

class Person{
  private:
  string name[MAX];
  int age[MAX];
  int last = -1;

  public:

  void addRec(string n, int a){ //called a method not function
    if(isFull()){
      cout << "Record is full." << endl;
    }
    else{
      last++;
      name[last] =  n;
      age[last] = a;
    }
  }
  
  void delRec(string n){
    if(isEmpty()){
      cout << "Record is empty. There is nothing to delete." << endl;
    }
    else{
      int p = searchPos(n);

      if(p == -1){
        cout << n << "'s record not found." << endl;
      }
      else{
        for(int i = p; i <= last ; i++){
            name[last] = name[last+1];
            age[last] = age[last+1];
        }
        last--;
        cout << "Record successfully deleted." << endl;
        system("pause");
      }
    }
  }

  void display(){
    int i;
    int border = 40;
    for(i = 0; i < border; i++) cout << "=";
    //cout << endl;
    cout << "\n" << std::left << setw(4) << "NO.";
    cout << std::left << setw(15) << "NAME";
    cout << std::left << setw(10) << "AGE";
    cout << std::left << setw(10) << "REMARKS" << endl;
    for(i = 0; i < border; i++) cout << "=";
    cout << endl;
    
    for(i = 0; i <= last; i++){
      cout << std::left << setw(4) << i+1;
      cout << std::left << setw(15) << name[i];
      cout << std::left << setw(10) << age[i];
      cout << std::left << setw(10) << (age[i] >= 18? "Adult":"Minor") << endl; 
    }
    system("pause");
  }

  int searchPos(string n){
    int i;
    for(int i = 0; i <= last; i++){
      if(name[i] == n){
        return i;
      }
    }
    return -1;
  }

  bool isEmpty(){
    return last == -1;
  }

  bool isFull(){
    return last == MAX -1;
  }

  void save(){
    int i;
    ofstream fp(file);

    for(i = 0; i <= last; i++){
      fp << name[i] << ", " << age[i] << endl;
    }
    fp.close();
  }

  void retrieve(){
    ifstream fp(file);
    string line;

    while(getline(fp, line)){
      stringstream ss(line); //ss writes the data of line in a string
      string nm, ageStr;

      if(getline(ss, nm, ',') && getline(ss, ageStr)){
        int age = stoi(ageStr);

        if(!nm.empty()){
          addRec(nm, age);
        }
      }
    }
    fp.close();
  }

  int menu(){
    int i, ch;
    int border = 35;
    for(i = 0; i <= border; i++) cout << "=";
    cout << "\n\tMENU" << endl;
    for(i = 0; i <= border; i++) cout << "=";
    cout << "\n[1] ADD RECORD" << endl;
    cout << "[2] DELETE RECORD" << endl;
    cout << "[3] DISPLAY ALL RECORD" << endl;
    cout << "[4] EXIT" << endl;
    cout << "Select(1-4): ";
    cin >> ch;

    return ch;
  }
};

int main(){
  string name;
  int age;
  Person prs; //create a Person object called prs

  prs.retrieve();
  while(1){
    switch(prs.menu()){
      case 1:
        cout << "Enter name: ";
        cin.ignore();
        getline(cin, name);
        
        cout << "Enter age: ";
        cin >> age;
        
        prs.addRec(name, age);
        break;
        
      case 2:
        cout << "Enter name: ";
        cin.ignore();
        getline(cin, name);

        prs.delRec(name);
        break;

      case 3:
        prs.display();
        break;

      case 4:
        prs.save();
        cout << "Saving data..." << endl;
        exit(0);

      default:
      while(getchar() != '\n');
        cout << "Invalid Input." << endl;
    }
  }
  return 0;
}