// Project 1: ATM System
// BSCS 2A 
// Programmed by: Romero, Shylla Mae L.
//                Tumala, Samantha Michelle G.

#include <iostream>
#include <windows.h>
#include <sstream>
#include <iomanip>
#include <fstream>
#include <string>
#include <cstdlib>
#include <limits>
using namespace std;

struct Account{
  string accountNum;
  string name;
  string birthday;
  string contactNum;
  float balance;
  string pin;
  string encryptedPin;
  Account *next;
};

class ATM{
  private:
    Account *head;
    const int CAESAR_SHIFT = 3;
    long nextAccountNum = 10000;
  public:
    ATM(){
      head=NULL;
    }

  void addAccount(string accNum, string name, string bday, string contNum, float balance, string pin);
  void accountRecords();
  Account *find(string accNum);
  void updateRecord(string pin);
  string generateAccountNumber();
  string caesarEncrypt(string pin);
  string caesarDecrypt(string pin);
  string findCardDrive();
  string findRemovableDrive();
  bool saveToCard(const string& drive, string& accNum, string& pin);
  bool retrieveCardData(string drive, string accNum, string pin);
  string amountFormat(float amt);
  string trim(string s);
  bool isValidName(string s);
  bool isValidBirthday(string s);
  bool isValidContact(string s);
  bool isValidPIN(string s);
  bool isValidDeposit(float amount);
  bool isValidWithdraw(float amount, float balance);
  bool isDuplicateAccount(string name, string bday, string contNum);
  Account *registerAccount(string &outAccNum, string &outPin, string &outDrive);
  Account *login(string &outAccNum, string &outPin, string &outDrive);
  Account *authenticate(string &outAccNum, string &outPin, string &outDrive);
};

void ATM::addAccount(string accNum, string name, string bday, string contNum, float balance, string pin){
  Account *node = new Account();
  node->accountNum = accNum;
  node->name = name;
  node->birthday = bday;
  node->contactNum = contNum;
  node->balance = balance;
  node->pin = pin;
  node->encryptedPin = caesarEncrypt(pin);
  node->next = head;
  head = node;
}

void ATM::accountRecords(){
  ifstream file("Account_Records.csv");
  string line;
  long maxAccNum = 0;

  if(!file.good()){
    cout << "Warning: could not open ATM Record. Machine out of service.\n";
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
      double balance = stod(balanceStr);
      addAccount(accNum, name, bday, contNum, balance, pin);
      long numericAccNum = stol(accNum);
      if(numericAccNum > maxAccNum) maxAccNum = numericAccNum;
    } catch(...) {
      continue; //likely a stray/header/blank row, skip it instead of crashing
    }
  }
  if(maxAccNum >= nextAccountNum) nextAccountNum = maxAccNum;
}

Account *ATM::find(string accNum){
  Account *p = head;
  while(p != NULL){
    if(p->accountNum == accNum){
      return p;
    }
    p = p->next;
  }
  return NULL;
}

void ATM::updateRecord(string pin){
  ofstream file("Account_Records.csv");
  if(!file.good()){
    cout << "could not save changes to ATM record." << endl;
  }
  else{
    Account *p;
    p=head;
    while(p!=NULL){
      file << p->accountNum << ", " << p->name << ", " << p->birthday << ", " << p->contactNum << ", " << p->balance << ", "<< p->pin << endl;
      p=p->next;
    }
  }
}

string ATM::generateAccountNumber(){
  nextAccountNum++;
  return to_string(nextAccountNum);
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

string ATM::findCardDrive(){
  DWORD drives = GetLogicalDrives();
  char letter;
  for(letter = 'A'; letter <= 'Z'; letter++){
    if(drives & 1){ //checks if the lowest bit from GetLogicalDrive has a drive plugged.
      string root = string(1, letter) + ":\\"; //for folder path
      if(GetDriveTypeA(root.c_str()) == DRIVE_REMOVABLE){ //converts cpp string to c string for compatibility and checks if the detected storage is removable rather than fixed drive of the computer.
        ifstream test(root + "pin.code");
        if(test.good()) return root;
      }
    }
    drives >>= 1;
  }
  return "";
}

string ATM::findRemovableDrive(){
  DWORD drives = GetLogicalDrives();
  char letter;
  for(letter = 'A'; letter <= 'Z'; letter++){
    if(drives & 1){
      string root = string(1, letter) + ":\\";
      if(GetDriveTypeA(root.c_str()) == DRIVE_REMOVABLE){
        return root;
      }
    }
    drives >>= 1;
  }
  return "";
}

bool ATM::saveToCard(const string& drive, string& accNum, string& pin){
  ifstream file(drive + "pin.code"); //tells program to read the pin.code file on the flash drive
  if(!file.good()){
    return false;
  }
  string encryptedPin, balanceStr;
  getline(file, accNum);
  getline(file, encryptedPin);
  accNum = trim(accNum);
  encryptedPin = trim(encryptedPin);
  if(accNum.empty() || encryptedPin.empty()){
    return false; // caller shows the message, so only one warning is ever printed
  }
  pin = caesarDecrypt(encryptedPin); // decrypt the PIN stored on the card
  return true;
}

bool ATM::retrieveCardData(string drive, string accNum, string pin){
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
  size_t start = s.find_first_not_of(" \t\r\n");
  size_t end   = s.find_last_not_of(" \t\r\n");
  if(start == string::npos) return "";
  return s.substr(start, end - start + 1);
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


Account *ATM::registerAccount(string &outAccNum, string &outPin, string &outDrive){
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
      cout << "Invalid contact number. Digits only, 7-13 digits.\n";
    }
  }

  if(isDuplicateAccount(name, bdayIn, contIn)){
    cout << "Warning: An account with this name, birthday, and contact number is already registered. Please Proceed to LogIn.\n";
    system("pause");
    return NULL;
  }

  string newAccNum = generateAccountNumber();
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

  addAccount(newAccNum, name, bdayIn, contIn, initialBalance, newPin);
  updateRecord(newPin); // save the new account to Account_Records.csv

  string regDrive;
  while(regDrive.empty()){
    regDrive = findRemovableDrive();
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

//REGISTER AND LOGIN
Account *ATM::authenticate(string &outAccNum, string &outPin, string &outDrive){
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
Account *ATM::login(string &outAccNum, string &outPin, string &outDrive){
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

  string accNum, name, bday, cardPin;
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
  if(enteredPin != cardPin){
    cout << "Incorrect PIN. Try Again.\n";
    system("pause");
    return NULL;
  }

  acc->pin = cardPin; // keep in-memory account in sync with the card

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
  string drive, accNum, cardPin;

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
        atm.updateRecord(acc->pin);
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
          atm.updateRecord(acc->pin);
          cout << "Deposit successful.\n";
        }
        break;
      }
      case 4: {
        string targetAccNum;
        float amt;
        cout << "Enter recipient account number: ";
        cin >> targetAccNum;
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
        cin >> amt;
        if(amt <= 0){
          cout << "Invalid transfer amount. Amount must be greater than zero.\n";
        } else if(amt > acc->balance){
          cout << "Insufficient funds.\n";
        } else {
          acc->balance -= amt;
          target->balance += amt;
          atm.updateRecord(target->pin);
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
        atm.updateRecord(newPin); // save the change to Account_Records.csv
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