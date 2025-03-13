#include <iostream>
#include <vector>
#include <map>
#include <memory>
#include <stdexcept>
using namespace std;

class Book {
public:
    string title, author, ISBN;
    bool isAvailable;

    Book(string t, string a, string i) : title(t), author(a), ISBN(i), isAvailable(true) {}
    void display() const {
        cout << "Title: " << title << ", Author: " << author << ", ISBN: " << ISBN << ", Available? " << (isAvailable ? "true" : "false") << endl;
    }
};

class User {
protected:
    static int idCounter;
public:
    string name;
    int userID;
    vector<string> borrowedBooks;

    User(string n) : name(n), userID(++idCounter) {}
    virtual void display() {
        cout << "User ID: " << userID << ", Name: " << name << "\nBooks Checked Out:";
        for (const auto& book : borrowedBooks) {
            cout << "\n-- ISBN: " << book;
        }
        cout << endl;
    }
    virtual ~User() {}
};

int User::idCounter = 0;

class Student : public User {
public:
    Student(string n) : User(n) {}
};

class Faculty : public User {
public:
    Faculty(string n) : User(n) {}
};

class UserFactory {
public:
    static unique_ptr<User> createUser(const string& type, const string& name) {
        if (type == "Student") return unique_ptr<Student>(new Student(name));
        else if (type == "Faculty") return unique_ptr<Faculty>(new Faculty(name));
        throw invalid_argument("Invalid user type");
    }
};

class Library {
private:
    static Library* instance;
    Library() {}
public:
    vector<Book> books;
    map<int, unique_ptr<User> > users;

    static Library* getInstance() {
        if (!instance) instance = new Library();
        return instance;
    }

    void addBook(string title, string author, string ISBN) {
        books.emplace_back(title, author, ISBN);
    }
    
    void registerUser(const string& type, const string& name) {
        unique_ptr<User> user = UserFactory::createUser(type, name);
        users[user->userID] = move(user);
    }
    
    void borrowBook(int userID, string title) {
        auto userIt = users.find(userID);
        if (userIt == users.end()) {
            cout << "Error: No User with that ID Exists\n";
            return;
        }
        for (auto& book : books) {
            if (book.title == title) {
                if (!book.isAvailable) {
                    cout << "Error: Book already borrowed\n";
                    return;
                }
                book.isAvailable = false;
                userIt->second->borrowedBooks.push_back(book.ISBN);
                cout << "> " << title << " checked out by User " << userID << "\n";
                return;
            }
        }
        cout << "Error: No book with that title exists\n";
    }
    
    void displayBooks() {
        cout << ">> List All Books\n>> -------\n";
        for (size_t i = 0; i < books.size(); i++) {
            cout << ">> Book " << i << ":\n";
            books[i].display();
        }
    }
    
    void displayUsers() {
        cout << ">> List All Users\n>> -------\n";
        for (auto& [id, user] : users) {
            cout << ">> User " << id << ":\n";
            user->display();
        }
    }
};

Library* Library::instance = nullptr;

void menu() {
    Library* lib = Library::getInstance();
    int choice;
    while (true) {
        cout << "\nWelcome to the Library:\n";
        cout << "1. Manage Books\n2. Manage Users\n3. Manage Transactions\n4. Exit\nEnter your choice: ";
        cin >> choice;
        cin.ignore();
        
        if (choice == 1) {
            int bookChoice;
            cout << "> Manage Books:\n1. Add a Book\n2. Go Back\nEnter your choice: ";
            cin >> bookChoice;
            cin.ignore();
            if (bookChoice == 1) {
                string title, author, ISBN;
                cout << "Enter the Title (0 to cancel): "; getline(cin, title);
                cout << "Enter the Author (0 to cancel): "; getline(cin, author);
                cout << "Enter the ISBN (0 to cancel): "; getline(cin, ISBN);
                if (title != "0" && author != "0" && ISBN != "0") {
                    lib->addBook(title, author, ISBN);
                    cout << "Book Added\n";
                }
            }
        } else if (choice == 2) {
            int userChoice;
            cout << "> Manage Users:\n1. Add a User\n2. Go Back\nEnter your choice: ";
            cin >> userChoice;
            cin.ignore();
            if (userChoice == 1) {
                string type, name;
                cout << "Enter Student or Faculty: "; cin >> type;
                cout << "Enter Name: "; cin.ignore(); getline(cin, name);
                lib->registerUser(type, name);
                cout << "User Added\n";
            }
        }
        else if (choice == 3) {
            int transactionChoice;
            cout << "> Manage Transactions:\n1. Check Out A Book\n2. Check In A Book\n3. List All Books\n4. List All Users\n5. Go Back\nEnter your choice: ";
            cin >> transactionChoice;
            cin.ignore();
            if (transactionChoice == 1) {
                string title;
                int userID;
                cout << ">> Book Title (or 0 to cancel): ";
                getline(cin, title);
                if (title == "0") return;
                cout << ">> User ID (or x to cancel): ";
                cin >> userID;
                if (cin.fail()) {
                    cin.clear();
                    cin.ignore();
                    cout << "Error: Invalid User ID\n";
                    return;
                }
                lib->borrowBook(userID, title);
            } else if (transactionChoice == 3) {
                lib->displayBooks();
            } else if (transactionChoice == 4) {
                lib->displayUsers();
            }
        } else if (choice == 4) {
            cout << "Thank you for using the Library System!\n";
            break;
        }
    }
}

int main() {
    menu();
    return 0;
}
