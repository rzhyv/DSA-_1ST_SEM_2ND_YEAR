// Project 1: ATM System
// BSCS 2A 
// Programmed by: Romero, Shylla Mae L.
//                Tumala, Samantha Michelle G.

#include <iostream> 
#include <windows.h> 
#include <sstream> 
#include <fstream> 
#include <iomanip> 
#include <string> 
#include <cstdlib>
#include <limits>
using namespace std;

struct Account{ 
  int accountNum;
  string name;
  string birthday;
  string contactNum;
  float balance;
  string pin;
  string encryptedPin;
  Account *next;
  Account(int accNum, string nm, string bday, string contNum, float bal, string PIN):
  accountNum(accNum), name(nm), birthday(bday), contactNum(contNum), balance(bal), pin(PIN), next(NULL){}
};

class ATM{
  private:
    Account *head; 
    const int CAESAR_SHIFT = 3; 
    int nextAccountNum = 10000;  
  public:
    ATM(){ 
      head=NULL;
    }
    ~ATM(){
      Account *p;
      while(p!=NULL){
        p=head;
        head=head->next;
        delete(p);
      }
    }
    void addAccount(int accNum, string name, string bday, string contNum, float balance, string pin);
    void accountRecords();
    Account *find(int accNum);
    void updateRecord(Account *acc);
    int generateAccountNumber();
    string caesarEncrypt(string pin);
    string caesarDecrypt(string pin);
    string findCardDrive(bool requirePinFile = true);
    bool saveToCard(const string& drive, int& accNum, string& pin);
    bool retrieveCardData(string drive, int accNum, string pin);
    string amountFormat(float amt);
    string trim(string s);
    bool isValidName(string s);
    bool isValidBirthday(string s);
    bool isValidContact(string s);
    bool isValidPIN(string s);
    bool isValidDeposit(float amount);
    bool isValidWithdraw(float amount, float balance);
    bool isDuplicateAccount(string name, string bday, string contNum);
    Account *registerAccount(int &outAccNum, string &outPin, string &outDrive);
    Account *login(int &outAccNum, string &outPin, string &outDrive);
    Account *authenticate(int &outAccNum, string &outPin, string &outDrive);
};

void ATM::addAccount(int accNum, string name, string bday, string contNum, float balance, string pin){
  Account *node = new Account(accNum, name, bday, contNum, balance, pin);
  node->encryptedPin = caesarEncrypt(pin);
  node->next = head;
  head = node;
}

void ATM::accountRecords(){
  ifstream file("Account_Records.csv");
  string line;
  long maxAccNum = 0;

  if(!file.good()){
    cout << "Warning: could not open ATM Record.\n";
    system("pause");
    return;
  }
  while(getline(file, line)){
    stringstream ss(line);
    string accNum, name, bday, contNum, balanceStr, pin;

    getline(ss, accNum, ',');
    getline(ss, name, ',');
    getline(ss, bday, ',');
    getline(ss, contNum, ',');
    getline(ss, balanceStr, ',');
    getline(ss, pin);
    
    accNum = trim(accNum);
    name = trim(name);
    bday = trim(bday);
    contNum = trim(contNum);
    pin = trim(pin);
    balanceStr = trim(balanceStr);

    try{
      float balance = stof(balanceStr); //type casting to float since it is the data type of balance in our account node
      int numericAccNum = stoi(accNum); //type casting to int
      addAccount(numericAccNum, name, bday, contNum, balance, pin);
      if(numericAccNum > maxAccNum) maxAccNum = numericAccNum;
    } catch(...) {
      continue;
    }
  }
  if(maxAccNum >= nextAccountNum) nextAccountNum = maxAccNum;
}

Account *ATM::find(int accNum){
  Account *p = head;
  while(p != NULL){
    if(p->accountNum == accNum){
      return p;
    }
    p = p->next;
  }
  return NULL;
}

void ATM::updateRecord(Account *acc){
  ofstream file("Account_Records.csv");
  if(!file.good()){
    cout << "could not save changes to ATM record." << endl;
  }
  else{
    Account *p;
    p=head;
    while(p!=NULL){
      file << p->accountNum << ", " << p->name << ", " << p->birthday << ", " << p->contactNum << ", " << amountFormat(p->balance) << ", "<< p->pin << endl; //store encrypted or plain pin?
      p=p->next;
    }
  }
}

int ATM::generateAccountNumber(){
  nextAccountNum++;
  return nextAccountNum;
}


string ATM::caesarEncrypt(string pin){
  string encrypted = pin;
  for(int i = 0; i < encrypted.length(); i++){
    if(isdigit(static_cast<unsigned char>(encrypted[i]))){
      int dig = encrypted[i] - '0';
      dig = (dig + CAESAR_SHIFT) % 10;
      encrypted[i] = static_cast<char>('0' + dig);
    }
  }
  return encrypted;
}

string ATM::caesarDecrypt(string pin){
  string decrypted = pin;
  for(int i = 0; i <decrypted.length(); i++){
    if(isdigit(static_cast<unsigned char>(decrypted[i]))){
      int dig = decrypted[i] - '0';
      dig = (dig - CAESAR_SHIFT + 10) % 10;
      decrypted[i] = static_cast<char>('0'+dig);
    }
  }
  return decrypted;
}

string ATM::findCardDrive(bool requirePinFile){
  DWORD drives = GetLogicalDrives();
  char letter;
  for(letter = 'A'; letter <= 'Z'; letter++){
    if(drives & 1){
      string root = string(1, letter) + ":\\";
      if(GetDriveTypeA(root.c_str()) == DRIVE_REMOVABLE){
        if(!requirePinFile) return root; // any removable drive is fine (e.g. for a fresh, unregistered card)
        ifstream test(root + "pin.code");
        if(test.good()) return root; // must already have a card file on it (e.g. for login)
      }
    }
    drives >>= 1;
  }
  return "";
}

bool ATM::saveToCard(const string& drive, int& accNum, string& pin){
  ifstream file(drive + "pin.code"); //tells program to read the pin.code file on the flash drive
  if(!file.good()){
    return false;
  }
  string accNumStr, encryptedPin;
  getline(file, accNumStr);
  getline(file, encryptedPin);
  accNumStr = trim(accNumStr);
  encryptedPin = trim(encryptedPin);
  if(accNumStr.empty() || encryptedPin.empty()){
    return false; // caller shows the message, so only one warning is ever printed
  }
  try{
    accNum = stoi(accNumStr); // text on the card -> int
  } catch(...) {
    return false; // card's account number isn't a valid number
  }
  pin = caesarDecrypt(encryptedPin); // decrypt the PIN stored on the card
  return true;
}

bool ATM::retrieveCardData(string drive, int accNum, string pin){
  ofstream file(drive + "pin.code");
  if(!file.good()){
    return false;
  }
  string encryptedPin = caesarEncrypt(pin);
  file << accNum << endl << encryptedPin;
  return true;
}

string ATM::amountFormat(float amt){ //for exactly 2 decimal in all balance
  stringstream ss;
  ss << fixed << setprecision(2) << amt;
  return ss.str();
}

string ATM::trim(string s){
  s.erase(0, s.find_first_not_of(" \t\r\n"));
  s.erase(s.find_last_not_of(" \t\r\n") + 1);
  return s;
}

// PIN CODE rule: max of 6 digits, minimum 4 digits, then press ENTER.
bool ATM::isValidName(string s){
  s = trim(s);
  if(s.empty()) return false;
  for(size_t i = 0; i < s.length(); i++){
    char c = s[i];
    if(!isalpha((unsigned char)c) && c != ' ' && c != '.' && c != '-' && c != '\'') return false;
  }
  return true;
}

// Strict MM-DD-YYYY format with basic range checks.
bool ATM::isValidBirthday(string s){
  s = trim(s);
  if(s.length() != 10 || s[2] != '-' || s[5] != '-') return false;
  for(size_t i = 0; i < s.length(); i++){
    if(i == 2 || i == 5) continue;
    if(!isdigit((unsigned char)s[i])) return false;
  }
  int month = stoi(s.substr(0, 2));
  int day   = stoi(s.substr(3, 2));
  int year  = stoi(s.substr(6, 4));
  if(month < 1 || month > 12) return false; // month must be 1-12
  if(day < 1 || day > 31) return false;
  if(year < 1900 || year > 2026) return false; // year capped at 2026
  if(month == 2 && day > 28) return false;
  if((month == 4 || month == 6 || month == 9 || month == 11) && day > 30) return false;
  return true;
}

bool ATM::isValidContact(string s){
  s = trim(s);
  if(s.length() != 11) return false;
  for(size_t i = 0; i < s.length(); i++){
    if(!isdigit((unsigned char)s[i])) return false;
  }
  return true;
}

// 4-6 digit number only. PIN VALIDATION
bool ATM::isValidPIN(string s){
  s = trim(s);
  if(s.length() < 4 || s.length() > 6) return false;
  for(size_t i = 0; i < s.length(); i++){
    if(!isdigit((unsigned char)s[i])) return false;
  }
  return true;
}

// Minimum deposit is P100.
bool ATM::isValidDeposit(float amount){
  return amount >= 100;
}

// Withdrawal must be a positive amount that does not exceed the balance
// (no negative amounts, no amounts that would drive the balance below 0).
bool ATM::isValidWithdraw(float amount, float balance){
  if(amount <= 0) return false;
  if(amount > balance) return false;
  return true;
}

// Prevents registering the same person twice: scans existing accounts
// for a matching name + birthday + contact number.
bool ATM::isDuplicateAccount(string name, string bday, string contNum){
  Account *p = head;
  while(p != NULL){
    if(p->name == name && p->birthday == bday && p->contactNum == contNum){
      return true;
    }
    p = p->next;
  }
  return false;
}


Account *ATM::registerAccount(int &outAccNum, string &outPin, string &outDrive){
  string name, newPin = "", confirmPin = " ", contIn, bdayIn;

  while(!isValidName(name)){
    cout << "--> Enter your name: ";
    getline(cin, name);
    if(!isValidName(name)){
      cout << "Invalid name. Letters and spaces only (no numbers/symbols), cannot be blank.\n";
    }
  }

  while(!isValidBirthday(bdayIn)){
    cout << "--> Enter your birthday (MM-DD-YYYY): ";
    getline(cin, bdayIn);
    if(!isValidBirthday(bdayIn)){
      cout << "Invalid birthday. Use the format MM-DD-YYYY (month 1-12, year up to 2026).\n";
    }
  }

  while(!isValidContact(contIn)){
    cout << "--> Enter your contact number: ";
    cin >> contIn;
    if(!isValidContact(contIn)){
      cout << "Invalid input. Contact number should only contain exactly 11 digits.\n";
    }
  }

  if(isDuplicateAccount(name, bdayIn, contIn)){
    cout << "Warning: An account with this name, birthday, and contact number is already registered. Please Proceed to LogIn.\n";
    system("pause");
    return NULL;
  }
  else{

  int newAccNum = generateAccountNumber();
  cout << "--> Your new account number is: " << newAccNum << "\n";

  while(newPin != confirmPin){
    while(!isValidPIN(newPin)){
      cout << "--> Set your PIN (4-6 digits): ";
      cin >> newPin;
      if(!isValidPIN(newPin)){
        cout << "Invalid PIN. Must be a 4-6-digit number.\n";
      }
    }

    confirmPin = ""; // reset so the inner loop always re-prompts for a fresh confirmation
    while(!isValidPIN(confirmPin)){
      cout << "--> Confirm your PIN (4-6 digits): ";
      cin >> confirmPin;
      if(!isValidPIN(confirmPin)){
        cout << "Invalid PIN. Must be a 4-6-digit number.\n";
      }
    }

    if(newPin != confirmPin){
      cout << "PINs do not match. Please try again.\n";
      newPin = ""; // reset both so the whole set/confirm pair is redone
      confirmPin = "";
    }
  }

  float initialBalance = 0;
  while(initialBalance < 5000){
    cout << "--> Enter initial balance: ";
    if(!(cin >> initialBalance)){
      cin.clear();
      cin.ignore(numeric_limits<streamsize>::max(), '\n');
      cout << "Invalid input. Numbers only.\n";
      continue;
    }
    if(initialBalance < 5000) cout << "Initial balance should not be lower than P5,000.\n"; 
  }

    Account *newAcc = find(newAccNum);
    addAccount(newAccNum, name, bdayIn, contIn, initialBalance, newPin);
    updateRecord(newAcc); // save the new account to Account_Records.csv
    
    string regDrive;
    while(regDrive.empty()){
      regDrive = findCardDrive(false);
      if(regDrive.empty()){
        Sleep(500); // wait briefly before checking again, no prompt shown
      }
    }
    
    retrieveCardData(regDrive, newAccNum, newPin);
    cout << "\nRegistration successful. Please proceed to login.\n";
    system("pause");
    
    outAccNum = newAccNum;
    outPin = newPin;
    outDrive = regDrive;
    return find(newAccNum);
  }
}

//REGISTER AND LOGIN
Account *ATM::authenticate(int &outAccNum, string &outPin, string &outDrive){
  Account *acc = NULL;

  while(!acc){
    int mainChoice;
    do{
      system("cls");
      cout << "Welcome to the ATM System!\n";
      cout << "\n1. Login (Insert Card)\n2. Register New Account\nEnter choice [1-2] : ";
      if(!(cin >> mainChoice)){
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Please enter 1 or 2 only.\n";
        system("pause");
        mainChoice = 0;
        continue;
      }
      cin.ignore(numeric_limits<streamsize>::max(), '\n');
      if(mainChoice != 1 && mainChoice != 2){
        cout << "Invalid choice. Please enter 1 or 2 only.\n";
        system("pause");
      }
    } while(mainChoice != 1 && mainChoice != 2);

    if(mainChoice == 2){
      acc = registerAccount(outAccNum, outPin, outDrive);
    } else {
      acc = login(outAccNum, outPin, outDrive);
    }
  }

  return acc;
}

// Choice 1: insert card, read it, and ask for the PIN. Returns the
// authenticated Account, or NULL if the card/PIN attempt failed.
Account *ATM::login(int &outAccNum, string &outPin, string &outDrive){
  string drive;
  while(drive.empty()){
    system("cls");
    cout << "Insert your ATM Card (Flash Drive) and press Enter...";
    cin.get();
    system("cls");
    drive = findCardDrive();
    if(drive.empty()){
      cout << "No card detected. Try Again.\n";
      system("pause");
    }
  }

  int accNum;
  string name, bday, cardPin;
  string contNum;
  float balance;
  if(!saveToCard(drive, accNum, cardPin)){
    cout << "Could not read your card. It may be unreadable, or its information doesn't match our records. Try Again.\n";
    system("pause");
    return NULL;
  }

  Account *acc = find(accNum);
  if(!acc){
    cout << "Account not found. Try Again.\n";
    system("pause");
    return NULL;
  }

  string enteredPin;
  cout << "Enter PIN: ";
  cin >> enteredPin;
  if(enteredPin != acc->pin){
    cout << "Incorrect PIN. Try Again.\n";
    system("pause");
    return NULL;
  }

  outAccNum = accNum;
  outPin = cardPin;
  outDrive = drive;
  return acc;
}

int menu(string name){
  int choice;
  system("cls");
  cout << "Hello, " << name << "!\n";
  cout << "\n===== ATM MENU =====\n";
  cout << "1. Balance Inquiry\n";
  cout << "2. Withdraw\n";
  cout << "3. Deposit\n";
  cout << "4. Fund Transfer\n";
  cout << "5. Change PIN Code\n";
  cout << "6. Exit\n";
  cout << "Enter choice: ";
  while(!(cin >> choice)){
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cout << "Invalid input. Please enter a number: ";
  }
  return choice;
}


int main(){
  ATM atm;
  atm.accountRecords();

  Account *acc = NULL;
  string drive, cardPin;
  int accNum = 0;

  acc = atm.authenticate(accNum, cardPin, drive);

  bool running = true;
  while(running){
    int choice = menu(acc->name);
    switch(choice){
      case 1:
        cout << "Balance: " << atm.amountFormat(acc->balance) << endl;
        break;
      case 2: {
        float amt;
        cout << "Amount to withdraw: ";
        if(!(cin >> amt)){
          cin.clear();
          cin.ignore(numeric_limits<streamsize>::max(), '\n');
          cout << "Invalid input. Numbers only.\n";
          break;
        }
        if(!atm.isValidWithdraw(amt, acc->balance)){
          if(amt <= 0){
            cout << "Invalid withdrawal amount. Amount must be greater than zero.\n";
          } else {
            cout << "Insufficient funds.\n";
          }
          break;
        }

        bool willBeZero = (amt == acc->balance);
        if(willBeZero){
          char confirm;
          cout << "Warning: Withdrawing this amount will leave your account with a zero balance. Continue? (Y/N): ";
          cin >> confirm;
          if(confirm != 'Y' && confirm != 'y'){
            cout << "Withdrawal cancelled.\n";
            break;
          }
        }

        acc->balance -= amt;
        atm.updateRecord(acc);
        cout << "Withdrawal successful.\n";
        if(willBeZero){
          cout << "Warning: Your account now has a zero balance.\n";
        }
        break;
      }
      case 3: {
        float amt;
        cout << "Amount to deposit: ";
        if(!(cin >> amt)){
          cin.clear();
          cin.ignore(numeric_limits<streamsize>::max(), '\n');
          cout << "Invalid input. Numbers only.\n";
          break;
        }
        if(!atm.isValidDeposit(amt)){
          cout << "Invalid deposit. Minimum deposit is P100.\n";
        } else {
          acc->balance += amt;
          atm.updateRecord(acc);
          cout << "Deposit successful.\n";
        }
        break;
      }
      case 4: {
        int targetAccNum;
        float amt;
        cout << "Enter recipient account number: ";
        if(!(cin >> targetAccNum)){
          cin.clear();
          cin.ignore(numeric_limits<streamsize>::max(), '\n');
          cout << "Invalid input. Numbers only.\n";
          break;
        }
        Account *target = atm.find(targetAccNum);
        if(!target){
          cout << "Recipient account not enrolled.\n";
          break;
        }
        if(target == acc){
          cout << "Cannot transfer to your own account.\n";
          break;
        }
        cout << "Amount to transfer: ";
        if(!(cin >> amt)){
          cin.clear();
          cin.ignore(numeric_limits<streamsize>::max(), '\n');
          cout << "Invalid input. Numbers only.\n";
          break;
        }
        if(amt <= 0){
          cout << "Invalid transfer amount. Amount must be greater than zero.\n";
        } else if(amt > acc->balance){
          cout << "Insufficient funds.\n";
        } else {
          acc->balance -= amt;
          target->balance += amt;
          atm.updateRecord(target);
          atm.updateRecord(acc);
          cout << "Transfer has been successful.\n";
        }
        break;
      }
      case 5: {
        string newPin = "", confirmPin;
        bool validPin = false;
        while(!validPin){
          cout << "Enter new PIN: ";
          cin >> newPin;
          if(!atm.isValidPIN(newPin)){
            cout << "New pin is invalid.\n";
          }
          else if(newPin == acc->pin){
            cout << "Old pin cannot be the same as new pin.\n";
          }
          else{
            validPin = true;
          }
        }
        while(newPin != confirmPin){
          cout << "Confirm new PIN: ";
          cin >> confirmPin;
          if(newPin != confirmPin){
            cout << "PINs do not match. Please try again.\n";
          }
        }
        acc->pin = newPin;
        acc->encryptedPin = atm.caesarEncrypt(newPin);
        atm.updateRecord(acc); // save the change to Account_Records.csv
        if(atm.retrieveCardData(drive, accNum, newPin)){ // save the change to pin.code
          cout << "PIN changed successfully.\n";
        } else {
          cout << "PIN changed, but could not write to the card. Please reinsert your card.\n";
        }
        break;
      }
      case 6:
        cout << "Thank you for using our ATM. Goodbye!\n";
        running = false;
        break;
      default:
        cout << "Invalid choice.\n";
    }
    if(choice != 6){
      system("pause");
    }
  }
  return 0;
}