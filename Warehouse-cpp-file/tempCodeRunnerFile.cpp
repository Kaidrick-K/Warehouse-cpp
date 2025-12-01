
#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <limits>
#include <fstream>
#include <sstream>
using namespace std;

// ======================= COLOR DEFINE ==========================
#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define CYAN    "\033[36m"
#define MAGENTA "\033[35m"
#define BOLD    "\033[1m"

// ======================== STRUCT ================================
struct Item {
    string name;
    int stock;
};

struct User {
    string username;
    string password;
    string role;
};

struct Request {
    int id;
    string username;
    string itemName;
    int quantity;
    string status;
};

// ======================== DATA STORAGE ===========================
vector<Item> items;
vector<User> users;
vector<Request> requests;
int requestCounter = 1;

// ======================== UI HELPERS =============================
void printLine(char c = '=') {
    cout << string(55, c) << "\n";
}

void headerUI(const string& title) {
    cout << MAGENTA;
    printLine('=');
    cout << CYAN << BOLD << "                 " << title << RESET << "\n";
    printLine('=');
    cout << RESET;
}

void pause() {
    cout << YELLOW << "\nTekan ENTER untuk kembali..." << RESET;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
}

// ---------- VALIDATION: Safe integer input ----------
int safeIntInput(int minOpt, int maxOpt) {
    int x;
    while (true) {
        cin >> x;
        if (!cin.fail() && x >= minOpt && x <= maxOpt) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return x;
        }

        cout << RED << "Input tidak valid! Masukkan angka " 
             << minOpt << "-" << maxOpt << "." << RESET << "\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Pilih: ";
    }
}

// ======================== SAVE & LOAD =============================

// ---------- SAVE USERS ----------
void saveUsers() {
    ofstream f("users.txt");
    for (auto &u : users) {
        f << u.username << "|" << u.password << "|" << u.role << "\n";
    }
    f.close();
}

// ---------- LOAD USERS ----------
void loadUsers() {
    users.clear();
    ifstream f("users.txt");
    if (!f.is_open()) return;

    string line;
    while (getline(f, line)) {
        if (line.empty()) continue;

        stringstream ss(line);
        string u, p, r;

        getline(ss, u, '|');
        getline(ss, p, '|');
        getline(ss, r, '|');

        users.push_back({u, p, r});
    }
    f.close();
}

// ---------- SAVE ITEMS ----------
void saveItems() {
    ofstream f("items.txt");
    for (auto &i : items) {
        f << i.name << "|" << i.stock << "\n";
    }
    f.close();
}

// ---------- LOAD ITEMS ----------
void loadItems() {
    items.clear();
    ifstream f("items.txt");
    if (!f.is_open()) return;

    string line;
    while (getline(f, line)) {
        if (line.empty()) continue;

        stringstream ss(line);
        string name, stockStr;

        getline(ss, name, '|');
        getline(ss, stockStr, '|');

        items.push_back({name, stoi(stockStr)});
    }
    f.close();
}

// ---------- SAVE REQUESTS ----------
void saveRequests() {
    ofstream f("requests.txt");
    for (auto &r : requests) {
        f << r.id << "|" << r.username << "|" << r.itemName
          << "|" << r.quantity << "|" << r.status << "\n";
    }
    f.close();
}

// ---------- LOAD REQUESTS ----------
void loadRequests() {
    requests.clear();
    ifstream f("requests.txt");
    if (!f.is_open()) return;

    string line;
    int maxID = 1;

    while (getline(f, line)) {
        if (line.empty()) continue;

        stringstream ss(line);
        string id, user, item, qty, stat;

        getline(ss, id, '|');
        getline(ss, user, '|');
        getline(ss, item, '|');
        getline(ss, qty, '|');
        getline(ss, stat, '|');

        int rid = stoi(id);
        maxID = max(maxID, rid);

        requests.push_back({rid, user, item, stoi(qty), stat});
    }

    requestCounter = maxID + 1;
    f.close();
}

// SAVE ALL
void saveAll() {
    saveUsers();
    saveItems();
    saveRequests();
}

// LOAD ALL
void loadAll() {
    loadUsers();
    loadItems();
    loadRequests();

    if (users.empty()) {
        users.push_back({"admin", "12345", "admin"});
        saveUsers();
    }
}

// ======================== FUNCTION HELPERS =======================
bool itemExists(const string& name) {
    for (auto &i : items) {
        if (i.name == name) return true;
    }
    return false;
}

Item* getItem(const string& name) {
    for (auto &i : items) {
        if (i.name == name) return &i;
    }
    return nullptr;
}

// ======================== CORE FUNCTIONS ==========================
void viewItems() {
    headerUI("DAFTAR STOK BARANG");

    cout << BOLD << BLUE << left << setw(25) << "Nama Barang" 
         << setw(10) << "Stok" << RESET << "\n";
    printLine('-');

    for (auto &i : items) {
        cout << left << setw(25) << i.name 
             << setw(10) << i.stock << "\n";
    }

    pause();
}

void addItem() {
    headerUI("TAMBAH BARANG");
    string name; 
    int stock;

    cin.ignore();
    cout << "Nama barang: ";
    getline(cin, name);

    if (itemExists(name)) {
        cout << RED << "Barang sudah ada! Gunakan menu tambah stok!\n" << RESET;
        pause();
        return;
    }

    cout << "Stok awal: ";
    cin >> stock;

    items.push_back({name, stock});
    saveItems();

    cout << GREEN << "Barang berhasil ditambahkan!\n" << RESET;
    pause();
}

void addStock() {
    headerUI("TAMBAH STOK (PRODUSEN)");
    cin.ignore();

    string name; 
    int add;

    cout << "Nama barang: ";
    getline(cin, name);

    Item* ptr = getItem(name);
    if (!ptr) {
        cout << RED << "Barang tidak ditemukan!\n" << RESET;
        pause();
        return;
    }

    cout << "Jumlah tambahan: ";
    cin >> add;

    ptr->stock += add;
    saveItems();

    cout << GREEN << "Stok berhasil ditambah!\n" << RESET;
    pause();
}

void requestItem(const string& username) {
    headerUI("REQUEST BARANG");
    cin.ignore();

    string name;
    int qty;

    cout << "Nama barang: ";
    getline(cin, name);

    Item* ptr = getItem(name);

    if (!ptr) {
        cout << RED << "Barang tidak tersedia!\n" << RESET;
        pause();
        return;
    }

    cout << "Jumlah: ";
    cin >> qty;

    if (qty <= 0) {
        cout << RED << "Jumlah tidak valid!\n" << RESET;
        pause();
        return;
    }

    if (ptr->stock < qty) {
        cout << RED << "Stok kurang!\n" << RESET;
        pause();
        return;
    }

    requests.push_back({requestCounter++, username, name, qty, "pending"});
    saveRequests();

    cout << GREEN << "Request berhasil dibuat!\n" << RESET;

    pause();
}

void viewUserRequests(const string& username) {
    headerUI("PERMINTAAN ANDA");

    for (auto &r : requests) {
        if (r.username == username) {
            cout << CYAN << "ID: " << r.id
                 << " | Item: " << r.itemName
                 << " | Qty: " << r.quantity
                 << " | Status: " << r.status << RESET << "\n";
        }
    }

    pause();
}

void viewAllRequests() {
    headerUI("SEMUA REQUEST");

    for (auto &r : requests) {
        cout << CYAN << "ID: " << r.id 
             << " | User: " << r.username 
             << " | Item: " << r.itemName 
             << " | Qty: " << r.quantity 
             << " | Status: " << r.status << RESET << "\n";
    }

    pause();
}

void processRequest() {
    headerUI("PROSES REQUEST");

    int id;
    cout << "ID request: ";
    cin >> id;

    for (auto &r : requests) {
        if (r.id == id) {

            Item* ptr = getItem(r.itemName);
            if (!ptr) {
                r.status = "rejected";
                saveRequests();
                cout << RED << "Barang tidak ditemukan!\n" << RESET;
                pause();
                return;
            }

            if (ptr->stock < r.quantity) {
                r.status = "rejected";
                saveRequests();
                cout << RED << "Stok kurang!\n" << RESET;
                pause();
                return;
            }

            ptr->stock -= r.quantity;
            r.status = "accepted";

            saveItems();
            saveRequests();

            cout << GREEN << "Request ACCEPTED!\n" << RESET;
            pause();
            return;
        }
    }

    cout << RED << "ID request tidak ditemukan!\n" << RESET;
    pause();
}

void viewAllUsers() {
    headerUI("DAFTAR USER TERDAFTAR");

    cout << BOLD << BLUE << left 
         << setw(20) << "Username" 
         << setw(15) << "Password" 
         << setw(10) << "Role" 
         << RESET << "\n";
    printLine('-');

    for (auto &u : users) {
        cout << left << setw(20) << u.username 
             << setw(15) << u.password 
             << setw(10) << u.role << "\n";
    }

    pause();
}

void addUser() {
    headerUI("TAMBAH USER");

    cin.ignore();
    string u, p, r;

    cout << "Username : "; getline(cin, u);
    cout << "Password : "; getline(cin, p);
    cout << "Role (admin/user): "; getline(cin, r);

    if (r != "admin" && r != "user") {
        cout << RED << "Role tidak valid!\n" << RESET;
        pause();
        return;
    }

    for (auto &us : users) {
        if (us.username == u) {
            cout << RED << "Username sudah digunakan!\n" << RESET;
            pause();
            return;
        }
    }

    users.push_back({u, p, r});
    saveUsers();

    cout << GREEN << "User berhasil ditambahkan!\n" << RESET;

    pause();
}

// ======================== MENU ADMIN =============================
void adminMenu() {
    while (true) {
        headerUI("ADMIN MENU");

        cout << "1. Lihat stok barang\n";
        cout << "2. Tambah barang\n";
        cout << "3. Tambah stok (produsen)\n";
        cout << "4. Lihat semua request\n";
        cout << "5. Proses request\n";
        cout << "6. Tambah user\n";
        cout << "7. Lihat semua user\n";
        cout << "8. Logout\n";
        cout << "Pilih: ";

        int c = safeIntInput(1, 8);

        if (c == 1) viewItems();
        else if (c == 2) addItem();
        else if (c == 3) addStock();
        else if (c == 4) viewAllRequests();
        else if (c == 5) processRequest();
        else if (c == 6) addUser();
        else if (c == 7) viewAllUsers();
        else if (c == 8) return;
    }
}

// ======================== MENU USER =============================
void userMenu(string username) {
    while (true) {
        headerUI("USER MENU");

        cout << "1. Lihat stok\n";
        cout << "2. Request barang\n";
        cout << "3. Lihat request saya\n";
        cout << "4. Logout\n";
        cout << "Pilih: ";

        int c = safeIntInput(1, 4);

        if (c == 1) viewItems();
        else if (c == 2) requestItem(username);
        else if (c == 3) viewUserRequests(username);
        else if (c == 4) return;
    }
}

// ======================== MENU GAADA USER =============================
void nouserMenu(string username) {
    while (true) {
        headerUI("Katalog");
        cout << "1. Lihat stok\n";
        cout << "2. Kembali\n";
        cout << "Pilih: ";

        int c = safeIntInput(1, 2);

        if (c == 1) viewItems();
        else if (c == 2) return;
    }
}

// ======================== LOGIN =============================
void login() {
    headerUI("LOGIN");
    cin.ignore();

    string u, p;
    cout << "Username: "; getline(cin, u);
    cout << "Password: "; getline(cin, p);

    for (auto &usr : users) {
        if (usr.username == u && usr.password == p) {
            if (usr.role == "admin") adminMenu();
            else userMenu(u);
            return;
        }
    }

    cout << RED << "Login gagal!\n" << RESET;
    pause();
}

// ======================== MAIN =============================
int main() {
    loadAll();

    while (true) {
        headerUI("MAIN MENU");

        cout << "1. Login\n";
        cout << "2. Keluar\n";
        cout << "3. Lihat Katalog\n";
        cout << "Pilih: ";

        int c = safeIntInput(1, 3);

        if (c == 1) login();
        else if (c == 3) nouserMenu() ;
        else if (c == 2); 
        {
            saveAll();
            break;
        }
    }
}
