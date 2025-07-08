#include <iostream>
#include <vector>
#include <string>
#include <limits>
#include <ctime>
using namespace std;

/* ---------- Transaction ---------- */
class Transaction {
public:
    string type;
    double amount;
    string date;

    Transaction(string t, double a) : type(move(t)), amount(a) {
        time_t now = time(nullptr);
        date = ctime(&now);
        date.pop_back();           // remove '\n'
    }
    void display() const {
        cout << type << " of ₹" << amount << " on " << date << '\n';
    }
};

/* ---------- Account ---------- */
class Account {
    static int nextAccountNumber;
public:
    int accountNumber;
    double balance{};
    vector<Transaction> transactions;

    Account() : accountNumber(++nextAccountNumber) {}

    void deposit(double amt) {
        balance += amt;
        transactions.emplace_back("Deposit", amt);
        cout << "₹" << amt << " deposited successfully.\n";
    }
    bool withdraw(double amt, bool silent = false) {
        if (amt > balance) {
            if (!silent) cout << "Insufficient balance.\n";
            return false;
        }
        balance -= amt;
        transactions.emplace_back("Withdrawal", amt);
        if (!silent) cout << "₹" << amt << " withdrawn successfully.\n";
        return true;
    }
    void showTransactions() const {
        cout << "\nTransaction History (A/C " << accountNumber << ")\n";
        if (transactions.empty()) cout << "No transactions yet.\n";
        else
            for (const auto& t : transactions) t.display();
    }
    void showBalance() const { cout << "Current Balance: ₹" << balance << '\n'; }
};
int Account::nextAccountNumber = 1000;

/* ---------- Customer ---------- */
class Customer {
public:
    string name;
    int customerID;
    Account account;

    Customer(string n, int id) : name(move(n)), customerID(id) {}

    void showInfo() const {
        cout << "\n── Account Info ──\n"
             << "Customer ID   : " << customerID << '\n'
             << "Name          : " << name << '\n'
             << "Account Number: " << account.accountNumber << '\n';
        account.showBalance();
    }
};

/* ---------- Helpers ---------- */
template <typename T>
bool safeInput(const string& prompt, T& var) {
    cout << prompt;
    cin >> var;
    if (cin.fail()) {
        cin.clear();                                       // reset failbit
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); // discard bad input
        cout << "Invalid input, please try again.\n";
        return false;
    }
    return true;
}
Customer* findCustomer(vector<Customer>& list, int id) {
    for (auto& c : list) if (c.customerID == id) return &c;
    return nullptr;
}

/* ---------- Main ---------- */
int main() {
    vector<Customer> customers;
    int nextCustomerID = 1;
    int choice;

    while (true) {
        cout << "\n====== Banking System Menu ======\n"
             << "1. Create New Customer\n2. Deposit Money\n3. Withdraw Money\n"
             << "4. Transfer Funds\n5. View Account Info\n6. View Transactions\n"
             << "7. Exit\nChoose: ";

        if (!safeInput("", choice)) continue;
        if (choice == 7) { cout << "🚪 Exiting… Thank you!\n"; break; }

        /* --- Choice-1: New Customer --- */
        if (choice == 1) {
            cout << "Enter customer name: ";
            cin.ignore();           // flush leftover newline
            string name;
            getline(cin, name);
            customers.emplace_back(name, nextCustomerID++);
            auto& nc = customers.back();
            cout << "Customer created! ID: " << nc.customerID
                 << ", Account No: " << nc.account.accountNumber << '\n';
            continue;
        }

        /* --- Remaining choices need a valid Customer first --- */
        int id;
        if (!safeInput("Enter customer ID: ", id)) continue;
        Customer* cust = findCustomer(customers, id);
        if (!cust) { cout << "Customer not found.\n"; continue; }

        /* --- Choice-2: Deposit --- */
        if (choice == 2) {
            double amt; if (!safeInput("Amount to deposit: ₹", amt)) continue;
            cust->account.deposit(amt);
        }
        /* --- Choice-3: Withdraw --- */
        else if (choice == 3) {
            double amt; if (!safeInput("Amount to withdraw: ₹", amt)) continue;
            cust->account.withdraw(amt);
        }
        /* --- Choice-4: Transfer --- */
        else if (choice == 4) {
            int targetID;
            if (!safeInput("Target customer ID: ", targetID)) continue;
            Customer* target = findCustomer(customers, targetID);
            if (!target) { cout << "Target customer not found.\n"; continue; }

            double amt; if (!safeInput("Amount to transfer: ₹", amt)) continue;

            if (cust->account.withdraw(amt, true)) { // silent withdraw
                target->account.deposit(amt);
                cout << "₹" << amt << " transferred successfully.\n";
            } else {
                cout << "Transfer failed (insufficient balance).\n";
            }
        }
        /* --- Choice-5: Info --- */
        else if (choice == 5) {
            cust->showInfo();
        }
        /* --- Choice-6: Transactions --- */
        else if (choice == 6) {
            cust->account.showTransactions();
        }
        else {
            cout << "Invalid option. Try again.\n";
        }
    }
    return 0;
}
