#pragma once
#include <string>
#include <iostream>
#include <fstream> 
#include <ctime> 
#include <queue>
#include <stack>
#include <unordered_set>
#include "hashTable.h" 
#include "doublyLinkedLIst.h" 
using namespace std;

class Graph {
private:
    struct UserNode {
        string userName;     
        string password;      
        string securityQuestion;
        string securityAnswer;   
        string lastLoginTime;    
        string lastLogoutTime;   
        DoublyLinkedList sharedFiles; 
        vector<UserNode*> sharedWith;
        struct SharedWithNode { 
            UserNode* user;
            SharedWithNode* next;
            SharedWithNode(UserNode* user) : user(user), next(nullptr) {}
        };

        SharedWithNode* sharedWithHead; 
        UserNode* next;               

        UserNode(const string& name, const string& pass = "", const string& question = "", const string& answer = "")
            : userName(name), password(pass), securityQuestion(question), securityAnswer(answer), sharedWithHead(nullptr), next(nullptr) {
        }
    };

    UserNode** table; 
    int tableSize;  
    const string userFile = "users.txt";
    int hashFunction(const std::string& key) const {
        int hash = 0;
        for (char ch : key) {
            hash = (hash * 31 + ch) % tableSize;
        }
        return hash;
    }
    std::string getCurrentTime() const {
        time_t now = time(nullptr);
        char buffer[26];
        ctime_s(buffer, sizeof(buffer), &now);
        return string(buffer);
    }
    void saveUserToFile(const UserNode* user) const {
        std::ofstream outFile(userFile, ios::app);
        if (outFile.is_open()) {
            outFile << user->userName << ","
                << user->password << ","
                << user->securityQuestion << ","
                << user->securityAnswer << endl;
            outFile.close();
        }
        else {
            std::cerr << "Error: Unable to open file for writing." << endl;
        }
    }
    void loadUsersFromFile() {
        ifstream inFile(userFile);
        if (inFile.is_open()) {
            string line;
            while (getline(inFile, line)) {
                size_t pos1 = line.find(',');
                size_t pos2 = line.find(',', pos1 + 1);
                size_t pos3 = line.find(',', pos2 + 1);

                if (pos1 != string::npos && pos2 != string::npos && pos3 != string::npos) {
                    string userName = line.substr(0, pos1);
                    string password = line.substr(pos1 + 1, pos2 - pos1 - 1);
                    string question = line.substr(pos2 + 1, pos3 - pos2 - 1);
                    string answer = line.substr(pos3 + 1);

                    addUser(userName, password, question, answer);
                }
            }
            inFile.close();
        }
        else {
            std::cerr << "Error: Unable to open file for reading." << endl;
        }
    }

public:
    Graph(int size = 10) : tableSize(size) {
        table = new UserNode * [tableSize];
        for (int i = 0; i < tableSize; ++i) {
            table[i] = nullptr;
        }
        loadUsersFromFile(); 
    }

    ~Graph() {
        for (int i = 0; i < tableSize; ++i) {
            UserNode* current = table[i];
            while (current) {
                UserNode* temp = current;
                current = current->next;
                delete temp;
            }
        }
        delete[] table;
    }

    UserNode* findUser(const string& user) const {
        int index = hashFunction(user);
        UserNode* current = table[index];
        while (current) {
            if (current->userName == user) {
                return current;
            }
            current = current->next;
        }
        return nullptr;
    }
    void addUser(const string& user, const string& pass = "", const string& question = "", const string& answer = "") {
        if (findUser(user)) {
            return; 
        }

        int index = hashFunction(user);
        UserNode* newUser = new UserNode(user, pass, question, answer);
        newUser->next = table[index];
        table[index] = newUser;
    }
    void signUp(const string& user, const string& pass, const string& question, const string& answer) {
        if (findUser(user)) {
            cout << "User already exists!" << endl;
            return;
        }

        addUser(user, pass, question, answer);
        saveUserToFile(findUser(user));
        cout << "User signed up successfully!" << endl;
    }
    void logIn(const string& user, const string& pass) {
        UserNode* userNode = findUser(user);
        if (!userNode) {
            std::cout << "User not found!" << endl;
            return;
        }

        if (userNode->password != pass) {
            cout << "Incorrect password!" << endl;
            return;
        }

        userNode->lastLoginTime = getCurrentTime();
        cout << "User logged in successfully at " << userNode->lastLoginTime;
    }
    void logOut(const string& user) {
        UserNode* userNode = findUser(user);
        if (!userNode) {
            cout << "User not found!" << endl;
            return;
        }

        userNode->lastLogoutTime = getCurrentTime();
        cout << "User logged out successfully at " << userNode->lastLogoutTime;
    }
    void recoverPassword(const string& user, const string& answer) {
        UserNode* userNode = findUser(user);
        if (!userNode) {
            cout << "User not found!" << endl;
            return;
        }

        if (userNode->securityAnswer != answer) {
            cout << "Incorrect answer to the security question!" << endl;
            return;
        }

        cout << "Your password is: " << userNode->password << endl;
    }
    void shareFile(const string& fromUser, const string& toUser, const string& fileName) {
        UserNode* fromNode = findUser(fromUser);
        UserNode* toNode = findUser(toUser);

        if (!fromNode || !toNode) {
            cout << "One or both users not found!\n";
            return;
        }
        fromNode->sharedFiles.addVersion(fileName);
        fromNode->sharedWith.push_back(toNode);

        cout << "File '" << fileName << "' shared from " << fromUser << " to " << toUser << ".\n";
    }
    void viewSharedFiles(const std::string& user) const {
        UserNode* userNode = findUser(user);
        if (!userNode || userNode->sharedFiles.getCurrentVersionContent().empty()) {
            cout << "No files shared by " << user << "." << endl;
            return;
        }

        cout << "Files shared by " << user << ":" << endl;
        userNode->sharedFiles.displayHistory();
    }
    void bfsTraversal(const string& startUser, const string& fileName) {
        UserNode* startNode = findUser(startUser);
        if (!startNode) {
            cout << "User not found!" << endl;
            return;
        }

        queue<UserNode*> q;
        unordered_set<std::string> visited;

        q.push(startNode);
        visited.insert(startNode->userName);

        cout << "BFS Traversal for file '" << fileName << "':" << endl;

        while (!q.empty()) {
            UserNode* current = q.front();
            q.pop();
            if (current->sharedFiles.getCurrentVersionContent() == fileName) {
                cout << current->userName << " has access to the file.\n";
            }
            for (UserNode* neighbor : current->sharedWith) {
                if (visited.find(neighbor->userName) == visited.end()) {
                    q.push(neighbor);
                    visited.insert(neighbor->userName);
                }
            }
        }
    }
    void dfsTraversal(const std::string& startUser, const std::string& fileName) {
        UserNode* startNode = findUser(startUser);
        if (!startNode) {
            cout << "User not found!\n";
            return;
        }

        stack<UserNode*> s;
        unordered_set<std::string> visited;

        s.push(startNode);
        visited.insert(startNode->userName);

        cout << "DFS Traversal for file '" << fileName << "':" << endl;

        while (!s.empty()) {
            UserNode* current = s.top();
            s.pop();
            if (current->sharedFiles.getCurrentVersionContent() == fileName) {
                cout << current->userName << " has access to the file.\n";
            }
            for (UserNode* neighbor : current->sharedWith) {
                if (visited.find(neighbor->userName) == visited.end()) {
                    s.push(neighbor);
                    visited.insert(neighbor->userName);
                }
            }
        }
    }
    void displayConnections() const {
        cout << "User Connections and Shared Files:\n";
        for (int i = 0; i < tableSize; ++i) {
            UserNode* current = table[i];
            while (current) {
                cout << current->userName << ":\n";
                current->sharedFiles.displayHistory();
                current = current->next;
            }
        }
    }
    void displayUsers() const {
        for (int i = 0; i < tableSize; ++i) {
            UserNode* current = table[i];
            while (current) {
                cout << "User: " << current->userName << "\n";
                cout << "Last Login: " << (current->lastLoginTime.empty() ? "Never" : current->lastLoginTime) << "\n";
                cout << "Last Logout: " << (current->lastLogoutTime.empty() ? "Never" : current->lastLogoutTime) << "\n";
                current = current->next;
            }
        }
    }
};