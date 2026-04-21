#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
using namespace std;

struct Account {
    int accNumber;
    string name;
    string type; // savings / current
    double balance;
};

void saveAccount(const Account& a) {
    ofstream f("accounts.dat", ios::app | ios::binary);
    f.write(reinterpret_cast<const char*>(&a), sizeof(Account));
}

bool findAccount(int accNo, Account& out, long& pos) {
    ifstream f("accounts.dat", ios::binary);
    if (!f) return false;
    pos = 0;
    while (f.read(reinterpret_cast<char*>(&out), sizeof(Account))) {
        if (out.accNumber == accNo) return true;
        pos += sizeof(Account);
    }
    return false;
}

void updateAccount(const Account& a, long pos) {
    fstream f("accounts.dat", ios::in | ios::out | ios::binary);
    f.seekp(pos);
    f.write(reinterpret_cast<const char*>(&a), sizeof(Account));
}

void createAccount() {
    Account a;
    cout << "\n--- Create Account ---\n";
    cout << "Account Number : "; cin >> a.accNumber;

    // check duplicate
    Account tmp; long pos;
    if (findAccount(a.accNumber, tmp, pos)) {
        cout << "Account already exists.\n";
        return;
    }

    cin.ignore();
    cout << "Holder Name    : "; getline(cin, a.name);
    cout << "Type (savings/current): "; cin >> a.type;
    cout << "Initial Deposit: "; cin >> a.balance;

    if (a.balance < 0) { cout << "Invalid amount.\n"; return; }

    saveAccount(a);
    cout << "Account created successfully.\n";
}

void displayAccount(const Account& a) {
    cout << "\n  Account No : " << a.accNumber
         << "\n  Name       : " << a.name
         << "\n  Type       : " << a.type
         << "\n  Balance    : $" << fixed << setprecision(2) << a.balance << "\n";
}

void viewAccount() {
    int accNo;
    cout << "\nEnter Account Number: "; cin >> accNo;
    Account a; long pos;
    if (!findAccount(accNo, a, pos)) { cout << "Account not found.\n"; return; }
    displayAccount(a);
}

void deposit() {
    int accNo; double amount;
    cout << "\nAccount Number : "; cin >> accNo;
    Account a; long pos;
    if (!findAccount(accNo, a, pos)) { cout << "Account not found.\n"; return; }
    cout << "Deposit Amount : "; cin >> amount;
    if (amount <= 0) { cout << "Invalid amount.\n"; return; }
    a.balance += amount;
    updateAccount(a, pos);
    cout << "Deposited $" << fixed << setprecision(2) << amount
         << ". New balance: $" << a.balance << "\n";
}

void withdraw() {
    int accNo; double amount;
    cout << "\nAccount Number : "; cin >> accNo;
    Account a; long pos;
    if (!findAccount(accNo, a, pos)) { cout << "Account not found.\n"; return; }
    cout << "Withdraw Amount: "; cin >> amount;
    if (amount <= 0 || amount > a.balance) { cout << "Invalid or insufficient funds.\n"; return; }
    a.balance -= amount;
    updateAccount(a, pos);
    cout << "Withdrawn $" << fixed << setprecision(2) << amount
         << ". New balance: $" << a.balance << "\n";
}

void transfer() {
    int fromAcc, toAcc; double amount;
    cout << "\nFrom Account : "; cin >> fromAcc;
    cout << "To Account   : "; cin >> toAcc;
    cout << "Amount       : "; cin >> amount;

    Account from, to; long posFrom, posTo;
    if (!findAccount(fromAcc, from, posFrom)) { cout << "Source account not found.\n"; return; }
    if (!findAccount(toAcc, to, posTo))       { cout << "Destination account not found.\n"; return; }
    if (amount <= 0 || amount > from.balance) { cout << "Invalid or insufficient funds.\n"; return; }

    from.balance -= amount;
    to.balance   += amount;
    updateAccount(from, posFrom);
    updateAccount(to,   posTo);
    cout << "Transferred $" << fixed << setprecision(2) << amount << " successfully.\n";
}

void deleteAccount() {
    int accNo;
    cout << "\nAccount Number to delete: "; cin >> accNo;

    ifstream fin("accounts.dat", ios::binary);
    if (!fin) { cout << "No accounts found.\n"; return; }

    ofstream fout("temp.dat", ios::binary);
    Account a; bool found = false;
    while (fin.read(reinterpret_cast<char*>(&a), sizeof(Account))) {
        if (a.accNumber == accNo) { found = true; continue; }
        fout.write(reinterpret_cast<const char*>(&a), sizeof(Account));
    }
    fin.close(); fout.close();

    if (!found) { cout << "Account not found.\n"; remove("temp.dat"); return; }
    remove("accounts.dat");
    rename("temp.dat", "accounts.dat");
    cout << "Account deleted.\n";
}

void listAllAccounts() {
    ifstream f("accounts.dat", ios::binary);
    if (!f) { cout << "No accounts found.\n"; return; }
    Account a; int count = 0;
    cout << "\n--- All Accounts ---\n";
    while (f.read(reinterpret_cast<char*>(&a), sizeof(Account))) {
        displayAccount(a);
        count++;
    }
    if (count == 0) cout << "No accounts found.\n";
    else cout << "\nTotal accounts: " << count << "\n";
}

int main() {
    int choice;
    do {
        cout << "\n====== Bank Management System ======\n"
             << "  1. Create Account\n"
             << "  2. View Account\n"
             << "  3. Deposit\n"
             << "  4. Withdraw\n"
             << "  5. Transfer\n"
             << "  6. Delete Account\n"
             << "  7. List All Accounts\n"
             << "  0. Exit\n"
             << "====================================\n"
             << "Choice: ";
        cin >> choice;

        switch (choice) {
            case 1: createAccount();   break;
            case 2: viewAccount();     break;
            case 3: deposit();         break;
            case 4: withdraw();        break;
            case 5: transfer();        break;
            case 6: deleteAccount();   break;
            case 7: listAllAccounts(); break;
            case 0: cout << "Goodbye.\n"; break;
            default: cout << "Invalid option.\n";
        }
    } while (choice != 0);

    return 0;
}
