#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <sstream>
#include <limits>
#include <ctime>

using namespace std;

// Smart Digital Expense Tracker with Budget Prediction
// Beginner-friendly C++ console project for academic demonstration.
// Data is stored in text files in the same folder as this program.

const int MAX_EXPENSES = 1000;
const string USER_FILE = "users.txt";
const string EXPENSE_FILE = "expenses.txt";
const string BUDGET_FILE = "budgets.txt";

struct Expense {
    int id;
    string username;
    string date;       // Format: YYYY-MM-DD
    string month;      // Format: YYYY-MM
    string category;
    string description;
    double amount;
};

struct Budget {
    string username;
    string month;      // Format: YYYY-MM
    double amount;
};

// ---------- Input helpers ----------
void clearInput() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

int readInt(const string& prompt, int minValue, int maxValue) {
    int value;
    while (true) {
        cout << prompt;
        if (cin >> value && value >= minValue && value <= maxValue) {
            clearInput();
            return value;
        }
        cout << "Invalid choice. Enter a number from " << minValue
             << " to " << maxValue << ".\n";
        clearInput();
    }
}

double readPositiveAmount(const string& prompt) {
    double value;
    while (true) {
        cout << prompt;
        if (cin >> value && value > 0) {
            clearInput();
            return value;
        }
        cout << "Please enter an amount greater than zero.\n";
        clearInput();
    }
}

string readNonEmptyLine(const string& prompt) {
    string value;
    do {
        cout << prompt;
        getline(cin, value);
        if (value.empty()) cout << "This field cannot be empty.\n";
    } while (value.empty());
    return value;
}

bool validDate(const string& date) {
    if (date.size() != 10 || date[4] != '-' || date[7] != '-') return false;
    for (int i = 0; i < 10; ++i) {
        if (i == 4 || i == 7) continue;
        if (date[i] < '0' || date[i] > '9') return false;
    }
    int month = stoi(date.substr(5, 2));
    int day = stoi(date.substr(8, 2));
    return month >= 1 && month <= 12 && day >= 1 && day <= 31;
}

string monthFromDate(const string& date) {
    return date.substr(0, 7);
}

// ---------- User registration and login ----------
bool usernameExists(const string& username) {
    ifstream file(USER_FILE);
    string savedUser, savedPassword;
    while (file >> quoted(savedUser) >> quoted(savedPassword)) {
        if (savedUser == username) return true;
    }
    return false;
}

void registerUser() {
    cout << "\n========== USER REGISTRATION ==========\n";
    string username = readNonEmptyLine("Create username: ");
    if (usernameExists(username)) {
        cout << "That username already exists. Please choose another.\n";
        return;
    }
    string password = readNonEmptyLine("Create password: ");
    ofstream file(USER_FILE, ios::app);
    if (!file) {
        cout << "Could not open the user data file.\n";
        return;
    }
    file << quoted(username) << ' ' << quoted(password) << '\n';
    cout << "Registration successful. You can now log in.\n";
}

bool loginUser(string& loggedInUser) {
    cout << "\n============== LOGIN ==============\n";
    string username = readNonEmptyLine("Username: ");
    string password = readNonEmptyLine("Password: ");

    ifstream file(USER_FILE);
    string savedUser, savedPassword;
    while (file >> quoted(savedUser) >> quoted(savedPassword)) {
        if (savedUser == username && savedPassword == password) {
            loggedInUser = username;
            cout << "Login successful. Welcome, " << username << "!\n";
            return true;
        }
    }
    cout << "Incorrect username or password.\n";
    return false;
}

// ---------- Expense file operations ----------
int loadExpenses(Expense expenses[]) {
    ifstream file(EXPENSE_FILE);
    int count = 0;
    Expense e;
    while (count < MAX_EXPENSES &&
           file >> e.id >> quoted(e.username) >> quoted(e.date)
                >> quoted(e.month) >> quoted(e.category)
                >> quoted(e.description) >> e.amount) {
        expenses[count++] = e;
    }
    return count;
}

void saveAllExpenses(const Expense expenses[], int count) {
    ofstream file(EXPENSE_FILE);
    if (!file) {
        cout << "Error: expenses could not be saved.\n";
        return;
    }
    for (int i = 0; i < count; ++i) {
        file << expenses[i].id << ' '
             << quoted(expenses[i].username) << ' '
             << quoted(expenses[i].date) << ' '
             << quoted(expenses[i].month) << ' '
             << quoted(expenses[i].category) << ' '
             << quoted(expenses[i].description) << ' '
             << fixed << setprecision(2) << expenses[i].amount << '\n';
    }
}

int nextExpenseId(const Expense expenses[], int count) {
    int largest = 0;
    for (int i = 0; i < count; ++i)
        if (expenses[i].id > largest) largest = expenses[i].id;
    return largest + 1;
}

// ---------- Budget file operations ----------
int loadBudgets(Budget budgets[], int maxCount) {
    ifstream file(BUDGET_FILE);
    int count = 0;
    Budget b;
    while (count < maxCount &&
           file >> quoted(b.username) >> quoted(b.month) >> b.amount) {
        budgets[count++] = b;
    }
    return count;
}

void saveAllBudgets(const Budget budgets[], int count) {
    ofstream file(BUDGET_FILE);
    if (!file) {
        cout << "Error: budgets could not be saved.\n";
        return;
    }
    for (int i = 0; i < count; ++i)
        file << quoted(budgets[i].username) << ' '
             << quoted(budgets[i].month) << ' '
             << fixed << setprecision(2) << budgets[i].amount << '\n';
}

double getBudget(const string& username, const string& month) {
    const int MAX_BUDGETS = 1000;
    Budget budgets[MAX_BUDGETS];
    int count = loadBudgets(budgets, MAX_BUDGETS);
    for (int i = 0; i < count; ++i)
        if (budgets[i].username == username && budgets[i].month == month)
            return budgets[i].amount;
    return 0;
}

void setMonthlyBudget(const string& username) {
    cout << "\n========== SET MONTHLY BUDGET ==========\n";
    string month = readNonEmptyLine("Enter month (YYYY-MM): ");
    if (month.size() != 7 || month[4] != '-') {
        cout << "Use the format YYYY-MM, for example 2026-11.\n";
        return;
    }
    double amount = readPositiveAmount("Enter budget amount (Rs.): ");

    const int MAX_BUDGETS = 1000;
    Budget budgets[MAX_BUDGETS];
    int count = loadBudgets(budgets, MAX_BUDGETS);
    bool updated = false;
    for (int i = 0; i < count; ++i) {
        if (budgets[i].username == username && budgets[i].month == month) {
            budgets[i].amount = amount;
            updated = true;
            break;
        }
    }
    if (!updated && count < MAX_BUDGETS) {
        budgets[count++] = {username, month, amount};
    } else if (!updated) {
        cout << "Budget storage is full.\n";
        return;
    }
    saveAllBudgets(budgets, count);
    cout << "Monthly budget saved successfully.\n";
}

// ---------- Expense features ----------
void addExpense(const string& username, Expense expenses[], int& count) {
    cout << "\n========== ADD DAILY EXPENSE ==========\n";
    if (count >= MAX_EXPENSES) {
        cout << "Expense storage is full.\n";
        return;
    }

    string date = readNonEmptyLine("Enter date (YYYY-MM-DD): ");
    if (!validDate(date)) {
        cout << "Invalid date format. Example: 2026-11-03\n";
        return;
    }
    cout << "Categories: Food, Travel, Shopping, Education, Bills, Health, Other\n";
    string category = readNonEmptyLine("Enter category: ");
    string description = readNonEmptyLine("Enter description: ");
    double amount = readPositiveAmount("Enter amount (Rs.): ");

    expenses[count] = {nextExpenseId(expenses, count), username, date,
                       monthFromDate(date), category, description, amount};
    ++count;
    saveAllExpenses(expenses, count);
    cout << "Expense added successfully. Expense ID: " << expenses[count - 1].id << '\n';
}

void printExpenseHeader() {
    cout << left << setw(6) << "ID" << setw(13) << "Date"
         << setw(16) << "Category" << setw(24) << "Description"
         << right << setw(12) << "Amount" << '\n';
    cout << string(71, '-') << '\n';
}

void viewExpenseHistory(const string& username, const Expense expenses[], int count) {
    cout << "\n========== EXPENSE HISTORY ==========\n";
    printExpenseHeader();
    bool found = false;
    for (int i = 0; i < count; ++i) {
        if (expenses[i].username == username) {
            cout << left << setw(6) << expenses[i].id
                 << setw(13) << expenses[i].date
                 << setw(16) << expenses[i].category
                 << setw(24) << expenses[i].description
                 << right << setw(12) << fixed << setprecision(2)
                 << expenses[i].amount << '\n';
            found = true;
        }
    }
    if (!found) cout << "No expenses found.\n";
}

void categoryWiseExpense(const string& username, const Expense expenses[], int count) {
    cout << "\n========== CATEGORY-WISE EXPENSE ==========\n";
    string categories[100];
    double totals[100] = {0};
    int categoryCount = 0;

    for (int i = 0; i < count; ++i) {
        if (expenses[i].username != username) continue;
        int position = -1;
        for (int j = 0; j < categoryCount; ++j)
            if (categories[j] == expenses[i].category) position = j;
        if (position == -1 && categoryCount < 100) {
            position = categoryCount++;
            categories[position] = expenses[i].category;
        }
        if (position != -1) totals[position] += expenses[i].amount;
    }

    if (categoryCount == 0) {
        cout << "No expenses available.\n";
        return;
    }
    for (int i = 0; i < categoryCount; ++i)
        cout << left << setw(20) << categories[i] << "Rs. "
             << fixed << setprecision(2) << totals[i] << '\n';
}

void monthlyExpenseReport(const string& username, const Expense expenses[], int count) {
    cout << "\n========== MONTHLY EXPENSE REPORT ==========\n";
    string month = readNonEmptyLine("Enter month (YYYY-MM): ");
    double total = 0;
    int numberOfExpenses = 0;
    for (int i = 0; i < count; ++i) {
        if (expenses[i].username == username && expenses[i].month == month) {
            total += expenses[i].amount;
            ++numberOfExpenses;
        }
    }
    cout << "Month: " << month << "\nNumber of expenses: " << numberOfExpenses
         << "\nTotal spending: Rs. " << fixed << setprecision(2) << total << '\n';
    double budget = getBudget(username, month);
    if (budget > 0)
        cout << "Monthly budget: Rs. " << budget
             << "\nRemaining amount: Rs. " << budget - total << '\n';
    else
        cout << "No budget has been set for this month.\n";
}

void budgetWarning(const string& username, const Expense expenses[], int count) {
    cout << "\n========== BUDGET WARNING ==========\n";
    string month = readNonEmptyLine("Enter month (YYYY-MM): ");
    double budget = getBudget(username, month);
    if (budget <= 0) {
        cout << "No budget found. Set a budget first.\n";
        return;
    }
    double spent = 0;
    for (int i = 0; i < count; ++i)
        if (expenses[i].username == username && expenses[i].month == month)
            spent += expenses[i].amount;

    cout << "Budget: Rs. " << budget << "\nSpent: Rs. " << spent << '\n';
    if (spent >= budget)
        cout << "WARNING: Your monthly budget has been reached or exceeded.\n";
    else if (spent >= budget * 0.80)
        cout << "WARNING: You have used at least 80% of your monthly budget.\n";
    else
        cout << "Your spending is currently within the budget.\n";
}

void predictExpense(const string& username, const Expense expenses[], int count) {
    cout << "\n========== EXPENSE PREDICTION ==========\n";
    string month = readNonEmptyLine("Enter month (YYYY-MM): ");
    int daysPassed = readInt("How many days of this month have passed? ", 1, 31);
    int daysInMonth = readInt("How many days are in this month (28-31)? ", 28, 31);
    if (daysPassed > daysInMonth) {
        cout << "Days passed cannot exceed the number of days in the month.\n";
        return;
    }

    double spent = 0;
    for (int i = 0; i < count; ++i)
        if (expenses[i].username == username && expenses[i].month == month)
            spent += expenses[i].amount;

    double predicted = (spent / daysPassed) * daysInMonth;
    double budget = getBudget(username, month);
    cout << "Amount spent so far: Rs. " << fixed << setprecision(2) << spent
         << "\nAverage daily spending: Rs. " << spent / daysPassed
         << "\nPredicted monthly spending: Rs. " << predicted << '\n';
    if (budget > 0) {
        cout << "Monthly budget: Rs. " << budget << '\n';
        if (predicted > budget)
            cout << "Prediction: You may exceed your budget by Rs. " << predicted - budget << ".\n";
        else
            cout << "Prediction: You may remain within your budget.\n";
    } else {
        cout << "Set a monthly budget to compare this prediction with your limit.\n";
    }
    cout << "Note: This is a simple estimate based on average daily spending, not an AI forecast.\n";
}

void sortExpensesByAmount(const string& username, const Expense expenses[], int count) {
    cout << "\n========== SORT EXPENSES BY AMOUNT ==========\n";
    Expense sorted[MAX_EXPENSES];
    int ownCount = 0;
    for (int i = 0; i < count; ++i)
        if (expenses[i].username == username) sorted[ownCount++] = expenses[i];

    if (ownCount == 0) {
        cout << "No expenses available to sort.\n";
        return;
    }

    // Bubble sort: arrange this user's expenses from lowest to highest amount.
    for (int i = 0; i < ownCount - 1; ++i) {
        for (int j = 0; j < ownCount - i - 1; ++j) {
            if (sorted[j].amount > sorted[j + 1].amount) {
                Expense temp = sorted[j];
                sorted[j] = sorted[j + 1];
                sorted[j + 1] = temp;
            }
        }
    }

    printExpenseHeader();
    for (int i = 0; i < ownCount; ++i) {
        cout << left << setw(6) << sorted[i].id
             << setw(13) << sorted[i].date
             << setw(16) << sorted[i].category
             << setw(24) << sorted[i].description
             << right << setw(12) << fixed << setprecision(2)
             << sorted[i].amount << '\n';
    }
    cout << "Expenses sorted from lowest to highest amount.\n";
}

void searchExpense(const string& username, const Expense expenses[], int count) {
    cout << "\n========== SEARCH EXPENSE ==========\n";
    int id = readInt("Enter expense ID: ", 1, 1000000000);
    for (int i = 0; i < count; ++i) {
        if (expenses[i].username == username && expenses[i].id == id) {
            printExpenseHeader();
            cout << left << setw(6) << expenses[i].id
                 << setw(13) << expenses[i].date
                 << setw(16) << expenses[i].category
                 << setw(24) << expenses[i].description
                 << right << setw(12) << fixed << setprecision(2)
                 << expenses[i].amount << '\n';
            return;
        }
    }
    cout << "Expense ID not found.\n";
}

void deleteExpense(const string& username, Expense expenses[], int& count) {
    cout << "\n========== DELETE EXPENSE ==========\n";
    int id = readInt("Enter expense ID to delete: ", 1, 1000000000);
    for (int i = 0; i < count; ++i) {
        if (expenses[i].username == username && expenses[i].id == id) {
            for (int j = i; j < count - 1; ++j)
                expenses[j] = expenses[j + 1];
            --count;
            saveAllExpenses(expenses, count);
            cout << "Expense deleted successfully.\n";
            return;
        }
    }
    cout << "Expense ID not found.\n";
}

// ---------- Logged-in user menu ----------
void userDashboard(const string& username) {
    Expense expenses[MAX_EXPENSES];
    int count = loadExpenses(expenses);

    while (true) {
        cout << "\n=============================================\n";
        cout << "       SMART DIGITAL EXPENSE TRACKER\n";
        cout << "             Logged in: " << username << '\n';
        cout << "=============================================\n";
        cout << "1. Add Daily Expense\n";
        cout << "2. View Expense History\n";
        cout << "3. Category-wise Expense\n";
        cout << "4. Monthly Expense Report\n";
        cout << "5. Set Monthly Budget\n";
        cout << "6. Budget Warning\n";
        cout << "7. Expense Prediction\n";
        cout << "8. Search Expense\n";
        cout << "9. Delete Expense\n";
        cout << "10. Sort Expenses by Amount\n";
        cout << "11. Logout\n";
        cout << "---------------------------------------------\n";

        int choice = readInt("Enter your choice: ", 1, 11);
        switch (choice) {
            case 1: addExpense(username, expenses, count); break;
            case 2: viewExpenseHistory(username, expenses, count); break;
            case 3: categoryWiseExpense(username, expenses, count); break;
            case 4: monthlyExpenseReport(username, expenses, count); break;
            case 5: setMonthlyBudget(username); break;
            case 6: budgetWarning(username, expenses, count); break;
            case 7: predictExpense(username, expenses, count); break;
            case 8: searchExpense(username, expenses, count); break;
            case 9: deleteExpense(username, expenses, count); break;
            case 10: sortExpensesByAmount(username, expenses, count); break;
            case 11: cout << "Logged out successfully.\n"; return;
        }
    }
}

// ---------- Main program ----------
int main() {
    cout << "\n*********************************************\n";
    cout << "   SMART DIGITAL EXPENSE TRACKER\n";
    cout << "   With Budget Prediction\n";
    cout << "*********************************************\n";

    while (true) {
        cout << "\n=============== MAIN MENU ===============\n";
        cout << "1. User Registration\n";
        cout << "2. Login\n";
        cout << "3. Exit\n";
        int choice = readInt("Enter your choice: ", 1, 3);

        if (choice == 1) {
            registerUser();
        } else if (choice == 2) {
            string username;
            if (loginUser(username)) userDashboard(username);
        } else {
            cout << "Thank you for using Smart Digital Expense Tracker.\n";
            break;
        }
    }
    return 0;
}
