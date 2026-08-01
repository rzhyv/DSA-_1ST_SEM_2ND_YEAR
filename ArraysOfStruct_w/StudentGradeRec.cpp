#include <iostream>
#include <iomanip>
#include <fstream> //files
#include <sstream> //other string syntax for retrieving
#define MAX 5

using namespace std;

const string file = "Grade_Record.csv";

struct GradeRec{
  string name;
  int q1, q2, q3;

  GradeRec(){
    name = "";
    q1 = 0;
    q2 = 0;
    q3 = 0;
  }

  GradeRec(string nm, int Q1, int Q2, int Q3){
    name = nm;
    q1 = Q1;
    q2 = Q2;
    q3 = Q3;
  }
};

class Record{
  private:
    GradeRec gr[MAX];
    int last;

  public:
  Record(){
    last = -1;
  }

  int border = 20;

  void addRec(GradeRec gRec){
    if(isFull()){
      cout << "Record is full." << endl;
      system("pause");
    }
    else{
      last++;
      gr[last] = gRec;
    }
  }

  void delRec(string nm){
    if(isEmpty()){
      cout << "Record is empty." << endl;
    }
    else{
      int p = locate(nm);

      if(p == -1){
        cout << "Record of " << nm << " not found." << endl;
      }
      else{
        int i;
        for(i = p; i <= last; i++){
          gr[i] = gr[i+1];
        }
        last--;
        cout << nm << "'s record successfully deleted." << endl;
      }
    }
    system("pause");
  }

  void update(string nm){
    int i, p, mn = 0;

    p = locate(nm);

    if(p == -1){
      cout << nm << "'s record not found." << endl;
    }
    else{
      while(mn != 4){

        for(i = 0; i <=border; i++) cout << '=';
        cout << "\nUPDATE MENU" << endl;
        for(i = 0; i <=border; i++) cout << '=';

        cout << "\n[1] UPDATE QUIZ 1" << endl;
        cout << "[2] UPDATE QUIZ 2" << endl;
        cout << "[3] UPDATE QUIZ 3" << endl;
        cout << "[4] BACK TO MAIN MENU" << endl;
        cout << "Select(1-4): ";
        cin >> mn;

          switch(mn){
            case 1:
              cout << "Enter quiz 1 score: ";
              cin >> gr[p].q1;
              break;
            case 2:
              cout << "Enter quiz 2 score: ";
              cin >> gr[p].q2;
              break;
            case 3:
              cout << "Enter quiz 3 score: ";
              cin >> gr[p].q3;
              break;
            case 4:
              cout << "going back to main menu..." << endl;
              break;
            default:
              while(getchar() != '\n');
              cout << "Invalid input." << endl;
          }
      }
    }
    system("pause");
  }

  void display(){
    int i;
    float a;
    for(i = 0; i <border*4; i++) cout << "=";
    cout << "\n" << std::left << setw(4) << "NO.";
    cout << std::left << setw(15) << "NAME";
    cout << std::left << setw(10) << "QUIZ 1";
    cout << std::left << setw(10) << "QUIZ 2";
    cout << std::left << setw(10) << "QUIZ 3";
    cout << std::left << setw(10) << "AVERAGE";
    cout << std::left << setw(10) << "REMARKS" << endl;
    for(i = 0; i <border*4; i++) cout << "=";
    cout << endl;

    for(i = 0; i <= last; i++){
      a = ave(gr[i].q1, gr[i].q2, gr[i].q3);
      cout << std::left << setw(4) << i+1;
      cout << std::left << setw(15) << gr[i].name;
      cout << std::left << setw(10) << gr[i].q1;
      cout << std::left << setw(10) << gr[i].q2;
      cout << std::left << setw(10) << gr[i].q3;
      cout << std::left << setw(10) << fixed << setprecision(2) << a;
      cout << std::left << setw(10) << (a >= 75 ? "PASSED":"FAILED") << endl;
    }
    system("pause");
  }

  float ave(int q1, int q2, int q3){
    return float(q1 + q2 + q3) / 3.0;
  }

  int locate(string nm){
    int i;
    for(i = 0; i <= last; i++){
      if(gr[i].name == nm){
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

    for(i = 0; i <= last; i++){
      fp << gr[i].name << ", " << gr[i].q1 << ", " << gr[i].q2 << ", " << gr[i].q3 << endl;
    }
    fp.close();
  }

  void retrieve(){
    ifstream fp(file);
    string line;
    GradeRec temp;

    while(getline(fp, line)){
      stringstream ss(line);
      string nm, Q1, Q2, Q3;

      while(getline(ss, nm, ',') && getline(ss, Q1, ',') && getline(ss, Q2, ',') && getline(ss, Q3)){
        int q1 = stoi(Q1);
        int q2 = stoi(Q2); //type casting from string to int
        int q3 = stoi(Q3);

        temp.name = nm;
        temp.q1 = q1;
        temp.q2 = q2;
        temp.q3 = q3;

        if(!nm.empty()){
          addRec(temp);
        }
      }
    }
    fp.close();
  }

  int menu(){
    int i, ch;
    for(i = 0; i < border; i++) cout << "=";
    cout << "\n\tMENU" << endl;
    for(i = 0; i < border; i++) cout << "=";

    cout << "\n[1] ADD RECORD" << endl;
    cout << "[2] DELETE RECORD" << endl;
    cout << "[3] UPDATE RECORD" << endl;
    cout << "[4] VIEW ALL RECORD" << endl;
    cout << "[5] EXIT" << endl;
    cout << "Select(1-5): ";
    cin >> ch;

    return ch;
  }
};

int main(){

  GradeRec gr;
  Record rec;

  rec.retrieve();
  while(1){
    switch(rec.menu()){
      case 1:
        cout << "Enter name: ";
        cin.ignore();
        getline(cin, gr.name);

        if(rec.locate(gr.name) != -1){
          cout << "Duplicate. Record already exist." << endl;
          system("pause");
          break;
        }
        
        cout << "Enter quiz 1 score: ";
        cin >> gr.q1;
        
        cout << "Enter quiz 2 score: ";
        cin >> gr.q2;
        
        cout << "Enter quiz 3 score: ";
        cin >> gr.q3;
        
        rec.addRec(gr);
        break;
        
      case 2:
        cout << "Enter name: ";
        cin.ignore();
        getline(cin, gr.name);

        rec.delRec(gr.name);
        break;

      case 3:
        cout << "Enter name: ";
        cin.ignore();
        getline(cin, gr.name);

        rec.update(gr.name);
        break;

      case 4:
        rec.display();
        break;

      case 5:
        cout << "Saving data..." << endl;
        rec.save();
        exit(0);

      default:
        while(getchar() != '\n');
        cout << "Invalid Input." << endl;
        system("pause");
    }
  }

  return 0;
}