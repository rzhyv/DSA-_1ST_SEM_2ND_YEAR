#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <cstring>
#define MAX 5

using namespace std;

const string file = "Student_Record.csv";

void addRec(char n[], int Q1, int Q2, int Q3);
void delRec(char n[]);
void display();
bool isFull();
bool isEmpty();
int locate(char n[]);
void save();
void retrieve();
float ave();
int menu();

char name[MAX][50];
int q1[MAX], q2[MAX], q3[MAX];
int last = -1;

void addRec(char n[], int Q1, int Q2, int Q3){
  if(isFull()){
    cout << "Record is full." << endl;
    system("pause");
  }
  else{
    last++;
    strcpy(name[last], n);
    q1[last] = Q1;
    q2[last] = Q2;
    q3[last] = Q3;
  }
}

void delRec(char n[]){
  if(isEmpty()){
    cout << "Record is empty." << endl;
  }
  else{
    int i, p;

    p = locate(n);

    if(p == -1){
      cout << "Record not found" << endl;
    }
    else{
      for(i = p; i <last; i++){
        strcpy(name[i], name[i+1]);
        q1[i] = q1[i+1];
        q3[i] = q3[i+1];
      }
      last--;
      cout << "Record Successfully deleted.";
    }
  }
  system("pause");
}

float ave(int q1, int q2, int q3){
  return float(q1 + q2 + q3) / 3.0;
}

void display(){
  int i, border = 40;
  float a;

  for(i = 0; i <border; i++) cout << "=";
  cout << endl;
  cout << std::left << setw(4) << "NO.";
  cout << std::left << setw(15) << "NAME";
  cout << std::left << setw(8) << "QUIZ 1";
  cout << std::left << setw(8) << "QUIZ 2";
  cout << std::left << setw(8) << "QUIZ 3";
  cout << std::left << setw(10) << "AVERAGE";
  cout << std::left << setw(10) << "REMARKS" << endl;
  for(i = 0; i <border; i++) cout << "=";
  cout << endl;

  for(i = 0; i <=last; i++){
    a = ave(q1[i], q2[i], q3[i]);
    cout << std::left << setw(4) << i+1;
    cout << std::left << setw(15) << name[i];
    cout << std::left << setw(8) << q1[i];
    cout << std::left << setw(8) << q2[i];
    cout << std::left << setw(8) << q3[i];
    cout << std::left << setw(10) << fixed << setprecision(2) << a;
    cout << std::left << setw(10) << (a >= 75 ? "PASSED":"FAILED") << endl;
  }
  system("pause");
}

int locate(char n[]){
  int i;
  for(i = 0; i <= last; i++){
    if(strcmp(name[i], n) == 0){
      return i;
    }
  }
  return -1;
}

bool isFull(){
  return last == MAX-1;
}

bool isEmpty(){
  return last == -1;
}

void save(){
  ofstream fp(file);
  int i;

  for(i = 0; i <=last; i++){
    fp << name[i] << ", " << q1[i] << ", " << q2[i] << ", " << q3[i] << endl;
  }
  fp.close();
}

void retrieve(){
  ifstream fp(file);
  string line;

  while(getline(fp, line)){
    stringstream ss(line);
    string nm, Q1s, Q2s, Q3s;

    while(getline(ss, nm, ',') && getline(ss, Q1s, ',') && getline(ss, Q2s, ',') && getline(ss, Q3s)){
      int Q1 = stoi(Q1s);
      int Q2 = stoi(Q2s);
      int Q3 = stoi(Q3s);

      if(!nm.empty()){
        char tempName[50];
        strcpy(tempName, nm.c_str());
        addRec(tempName, Q1, Q2, Q3);
      }
    }
  }
  fp.close();
}

int menu(){
  int ch;

  cout << "MENU" << endl;
  cout << "[1] ADD RECORD" << endl;
  cout << "[2] DELETE RECORD" << endl;
  cout << "[3] DISPLAY RECORD" << endl;
  cout << "[4] EXIT" << endl;
  cout << "Select(1-4): ";
  cin >> ch;

  return ch;
}

int main(){
  char name[50];
  int q1, q2, q3;
  retrieve();

  while(1){
    switch(menu()){
      case 1:
        cout << "Enter name: ";
        cin.ignore();
        cin.getline(name, 50);

        cout << "Enter quiz 1 score: ";
        cin >> q1;

        cout << "Enter quiz 2 score: ";
        cin >> q2;

        cout << "Enter quiz 3 score: ";
        cin >> q3;

        addRec(name, q1, q2, q3);
        break;

      case 2:
        cout << "Enter name: ";
        cin.ignore();
        cin.getline(name, 50);

        delRec(name);
        break;

      case 3:
        display();
        break;

      case 4:
        cout << "Saving record..." << endl;
        save();
        exit(0);
        break;

      default:
        while(getchar() != '\n');
        cout << "Invalid input." << endl;
        system("pause");
    }
  }

  return 0;
}
