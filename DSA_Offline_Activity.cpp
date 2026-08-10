#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#define MAX 5

using namespace std;

const string file = "Student_Record.csv";

struct Student{
  string name;
  int q1, q2, q3;
  
  Student(){
      name = "";
      q1 = 0;
      q2 = 0;
      q3 = 0;
  }
};

class Record{
  private:
  Student st[MAX];
  int last = -1;
  public:
  
  void addRec(Student stu){
    if(isFull()){
        cout << "Record is full." << endl;
        system("pause");
    }
    else{
      int i, pos = last+1;
      for(i = 0; i <= last; i++){
        if(st[i].name > stu.name){
          pos = i;
          break;
        }
      }

      for(i = last+1; i > pos; i--){
        st[i] = st[i-1];
      }
      st[pos] = stu;
      last++;
    }
  }
  
  void delRec(string nm){
    int i, p;
      if(isEmpty()){
        cout << "Record is empty." << endl;
      }
      else{
        p = locate(nm);
        
        if(p == -1){
          cout << "Record not found" << endl;
        }
        else{
          for(i = p; i < last; i++){
            st[i] = st[i+1];
          }
          last--;
          cout << "Record successfully deleted." << endl;
      }
    }
    system("pause");
  }

  void update(string nm){
    int i, p, up = 0;

    if(isEmpty()){
      cout << "Record is empty." << endl;
    }
    else{
      p = locate(nm);

      if(p == -1){
        cout << "Record not found." << endl;
      }
      else{
        system("cls");
        while(up != 4){
          int border = 30;
          float a = (st[p].q1 + st[p].q2 + st[p].q3) / 3.0;

          for(i=0; i<border; i++) cout << "=";
          cout << "\nCURRENT RECORD" << endl;
          for(i=0; i<border; i++) cout << "=";
          cout << "\nNAME: " << st[p].name << endl;
          cout << "QUIZ 1: " << st[p].q1 << endl;
          cout << "QUIZ 2: " << st[p].q2 << endl;
          cout << "QUIZ 3: " << st[p].q3 << endl;
          cout << "AVERAGE: " << fixed << setprecision(2) << a << "\n" << endl;


          for(i=0; i<border; i++) cout << "=";
          cout << "\nUPDATE MENU" << endl;
          for(i=0; i<border; i++) cout << "=";
          cout << "\n[1] UPDATE QUIZ 1 SCORE" << endl;
          cout << "[2] UPDATE QUIZ 2 SCORE" << endl;
          cout << "[3] UPDATE QUIZ 3 SCORE" << endl;
          cout << "[4] BACK TO MAIN MENU" << endl;
          cout << "Select(1-4): ";
          cin >> up;

          switch(up){
            case 1:
              cout << "Enter updated quiz 1 score: ";
              cin >> st[p].q1;
              break;

            case 2:
              cout << "Enter updated quiz 2 score: ";
              cin >> st[p].q2;
              break;

            case 3:
              cout << "Enter updated quiz 3 score: ";
              cin >> st[p].q3;
              break;

            case 4:
              cout << "Going back to main menu." << endl;
              break;

            default:
              while(getchar() != '\n');
              cin.clear();
              cout << "Invalid Input." << endl;
          }

        }
      }
    }
    system("pause");
  }
  
  void display(){
    system("cls");
    int i, border = 60;
    for(i = 0; i < border; i++) cout << "=";
    cout << endl;
    cout << std::left << setw(4) << "NO.";
    cout << std::left << setw(15) << "NAME";
    cout << std::left << setw(8) << "QUIZ 1";
    cout << std::left << setw(8) << "QUIZ 2";
    cout << std::left << setw(8) << "QUIZ 3";
    cout << std::left << setw(8) << "AVERAGE";
    cout << std::left << setw(8) << "REMARKS" << endl;
    for(i = 0; i < border; i++) cout << "=";
    cout << endl;
    
    float a;
    
    for(i = 0; i <= last; i++){
        a = float(st[i].q1 + st[i].q2 + st[i].q3) / 3.0;
        cout << std::left << setw(4) << i+1;
        cout << std::left << setw(15) << st[i].name;
        cout << std::left << setw(8) << st[i].q1;
        cout << std::left << setw(8) << st[i].q2;
        cout << std::left << setw(8) << st[i].q3;
        cout << std::left << setw(8) << fixed << setprecision(2) << a;
        cout << std::left << setw(8) << (a >= 75 ? "PASSED":"FAILED") << endl;
    }
    system("pause");
  }
  
  bool isFull(){
    return last == MAX -1;
  }
  
  bool isEmpty(){
    return last == -1;
  }
  
  int locate(string nm){
    int i;
    for(i = 0; i <= last; i++){
      if(st[i].name == nm){
          return i;
      }
    }
    return -1;
  }
  
  void save(){
    ofstream fp(file);
    int i;
    
    for(i = 0; i <= last; i++){
      fp << st[i].name << ", " << st[i].q1 << ", " << st[i].q2 << ", " << st[i].q3 << endl;
    }
    fp.close();
  }
  
  void retrieve(){
    ifstream fp(file);
    string line;
    Student temp;
    
    while(getline(fp, line)){
      stringstream ss(line);
      string nm, q1s, q2s, q3s;
      
      while(getline(ss, nm, ',') && getline(ss, q1s, ',') && getline(ss, q2s, ',') && getline(ss, q3s)){
        int Q1 = stoi(q1s);
        int Q2 = stoi(q2s);
        int Q3 = stoi(q3s);
        
        temp.name = nm;
        temp.q1 = Q1;
        temp.q2 = Q2;
        temp.q3 = Q3;
        
        if(!nm.empty()){
          addRec(temp);
        }
      }
    }
    fp.close();
  }
  
  int menu(){
    system("cls");
    int i, ch, border = 25;
    
    for(i = 0; i < border; i++) cout << "=";
    cout << "\n\t MENU" << endl;
    for(i = 0; i < border; i++) cout << "=";
    
    cout << "\n[1] ADD RECORD" << endl;
    cout << "[2] DELETE RECORD" << endl;
    cout << "[3] UPDATE RECORD" << endl;
    cout << "[4] DISPLAY ALL RECORD" << endl;
    cout << "[5] EXIT" << endl;
    cout << "Select(1-5): ";
    cin >> ch;
    
    return ch;
  }
};


int main() {
  Student st;
  Record rec;
  int p;
  
  rec.retrieve();
  while(1){
    switch(rec.menu()){
      case 1:
          cout << "Enter name: ";
          cin.ignore();
          getline(cin, st.name);

          p = rec.locate(st.name);
          if(p != -1){
            cout << "Duplicate record." << endl;
            system("pause");
            break;
          }
      
          cout << "Enter quiz 1 score: ";
          cin >> st.q1;
      
          cout << "Enter quiz 2 score: ";
          cin >> st.q2;
      
          cout << "Enter quiz 3 score: ";
          cin >> st.q3;
      
          rec.addRec(st);
          break;
        
      case 2:
          cout << "Enter name: ";
          cin.ignore();
          getline(cin, st.name);
      
          rec.delRec(st.name);
          break;

      case 3:
        cout << "Enter name: ";
        cin.ignore();
        getline(cin, st.name);

        rec.update(st.name);
        break;
      
      case 4:
          rec.display();
          break;
      
      case 5:
          cout << "Saving record..." << endl;
          rec.save();
          exit(0);
          break;
      
      default:
          while(getchar() != '\n');
          cin.clear();
          cout << "Invalid Input." << endl;
          system("pause");         
    }
  }
  return 0;
}