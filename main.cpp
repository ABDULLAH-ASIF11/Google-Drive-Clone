#include "googleDrive.h"
#include "hashTable.h"
#include "graph.h"
#include "dictionary.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <queue>
using namespace std;
#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
enum class UserRole {
    VIEWER,
    EDITOR,
    ADMIN
};
enum class Permission {
    READ = 1,
    WRITE = 2,
    EXECUTE = 4
};
bool hasPermission(UserRole role, Permission permission) {
    switch (role) {
    case UserRole::ADMIN:
        return true;
    case UserRole::EDITOR:
        return permission == Permission::READ || permission == Permission::WRITE;
    case UserRole::VIEWER:
        return permission == Permission::READ;
    default:
        return false;
    }
}

bool hasWriteAccess(UserRole role) {
    return role == UserRole::ADMIN || role == UserRole::EDITOR;
}
string compressRLE(const string& input) {
    if (input.empty()) return "";
    string compressed;
    char current = input[0];
    int count = 1;
    for (size_t i = 1; i < input.length(); i++) {
        if (input[i] == current) {
            count++;
        }
        else {
            compressed += to_string(count) + current;
            current = input[i];
            count = 1;
        }
    }
    compressed += to_string(count) + current;
    return compressed;
}
string decompressRLE(const string& input) {
    if (input.empty()) return "";

    string decompressed;
    for (size_t i = 0; i < input.length(); i += 2) {
        int count = input[i] - '0';
        char c = input[i + 1];
        decompressed.append(count, c);
    }
    return decompressed;
}
void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear"); 
#endif
}
void printSuccess(const string& message) {
    cout << GREEN << message << RESET << endl;
}
void printError(const string& message) {
    cout << RED << message << RESET << endl;
}
void printPrompt(const string& message) {
    cout << YELLOW << message << RESET << endl;
}
void showLoading(const string& message) {
    cout << message;
    for (int i = 0; i < 4; ++i) {
        cout << ".";
        this_thread::sleep_for(chrono::milliseconds(500));
    }
    cout << endl;
}
void showBreadcrumb(const string& currentPath) {
    cout << BLUE << "Current Path: " << currentPath << RESET << endl;
}
void displayMenu() {
    cout << "=====================================\n";
    cout << "         Google Drive Menu           \n";
    cout << "=====================================\n";
    cout << "1. Add Folder (Admin/Editor only)\n";
    cout << "2. Add File (Admin/Editor only)\n";
    cout << "3. Rename (Admin/Editor only)\n";
    cout << "4. Delete (Admin/Editor only)\n";
    cout << "5. Search\n";
    cout << "6. Display Structure\n";
    cout << "7. View Recycle Bin\n";
    cout << "8. Restore File (Admin/Editor only)\n";
    cout << "9. Show Help\n";
    cout << "10. View Recent Files\n";
    cout << "11. Add File Version (Admin/Editor only)\n";
    cout << "12. Rollback File Version (Admin/Editor only)\n";
    cout << "13. View File Version History\n";
    cout << "14. Exit\n";
    cout << "15. Create File (Admin/Editor only)\n";
    cout << "16. Read File\n";
    cout << "17. Update File (Admin/Editor only)\n";
    cout << "18. Delete File (Admin/Editor only)\n";
    cout << "19. Traverse Tree (BFS)\n";
    cout << "20. Traverse Tree (DFS)\n";
    cout << "21. Change User Role (Admin only)\n";
    cout << "22. Compress File (Admin/Editor only)\n";
    cout << "23. Decompress File (Admin/Editor only)\n";
    cout << "24. Queue Cloud Sync (Admin/Editor only)\n";
    cout << "=====================================\n";
}
void showHelp() {
    clearScreen();
    cout << "=====================================\n";
    cout << "              Help Menu              \n";
    cout << "=====================================\n";
    cout << "1. Add Folder: Add a new folder to the structure.\n";
    cout << "2. Add File: Add a new file to a folder with metadata (type, size, date, owner).\n";
    cout << "3. Rename: Rename a file or folder to a new name.\n";
    cout << "4. Delete: Delete a file or folder. Deleted files are moved to the Recycle Bin.\n";
    cout << "5. Search: Search for a file by name and view its metadata.\n";
    cout << "6. Display Structure: Show the folder structure in a tree format.\n";
    cout << "7. View Recycle Bin: View the most recently deleted file in the Recycle Bin.\n";
    cout << "8. Restore File: Restore the most recently deleted file from the Recycle Bin.\n";
    cout << "9. Show Help: Display this help menu with descriptions of all options.\n";
    cout << "10. View Recent Files: Display a list of recently accessed files.\n";
    cout << "11. Add File Version: Add a new version to a file's version history.\n";
    cout << "12. Rollback File Version: Rollback a file to a specific version from its version history.\n";
    cout << "13. View File Version History: View the version history of a file.\n";
    cout << "14. Exit: Exit the program.\n";
    cout << "15. Create File: Create a new file in the folder structure.\n"; 
    cout << "16. Read File: Read the content of a file.\n"; 
    cout << "17. Update File: Update the content of a file.\n"; 
    cout << "18. Delete File: Delete a file and move it to the Recycle Bin.\n"; 
    cout << "21. Change User Role: Change the role of a user (Viewer, Editor, Admin).\n";
    cout << "22. Compress File: Compress the content of a file using RLE.\n";
    cout << "23. Decompress File: Decompress the content of a file using RLE.\n";
    cout << "24. Queue Cloud Sync: Queue a file for synchronization to the cloud.\n";
    cout << "=====================================\n";
    cout << "Press Enter to go back to the main menu...";
    cin.ignore();
    cin.get();
}
bool confirmAction(const string& action) {
    char choice;
    cout << YELLOW << "Are you sure you want to " << action << "? (y/n): " << RESET;
    cin >> choice;
    return (choice == 'y' || choice == 'Y');
}
int getValidatedChoice() {
    int choice;
    while (true) {
        cout << "Enter your choice: ";
        cin >> choice;
        if (cin.fail() || choice < 1 || choice > 24) {
            cin.clear(); 
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            printError("Invalid choice! Please enter a number between 1 and 24.");
        }
        else {
            return choice;
        }
    }
}
void displayTreeWithSymbols(Node* node, int depth = 0) {
    if (!node) return;
    for (int i = 0; i < depth; ++i) cout << "  ";
    cout << (node->isFolder ? "[📁] " : "[📄] ") << node->name << endl;
    displayTreeWithSymbols(node->firstChild, depth + 1);
    displayTreeWithSymbols(node->nextSibling, depth);
}

bool logIn(Graph& userGraph, string& currentUser, UserRole& currentRole) {
    clearScreen();
    cout << "Please log in to continue.\n";
    string userName, password;
    cout << "Enter username: ";
    cin >> userName;
    cout << "Enter password: ";
    cin >> password;
    userGraph.logIn(userName, password);
    if (userGraph.findUser(userName)) {
        currentUser = userName;
        cout << "\nSelect your role:\n";
        cout << "1. Viewer (can only read files)\n";
        cout << "2. Editor (can read and modify files)\n";
        cout << "3. Admin (full access)\n";
        cout << "Enter your choice (1-3): ";

        int roleChoice;
        cin >> roleChoice;

        switch (roleChoice) {
        case 1:
            currentRole = UserRole::VIEWER;
            cout << "Logged in as Viewer\n";
            break;
        case 2:
            currentRole = UserRole::EDITOR;
            cout << "Logged in as Editor\n";
            break;
        case 3:
            currentRole = UserRole::ADMIN;
            cout << "Logged in as Admin\n";
            break;
        default:
            currentRole = UserRole::VIEWER;
            cout << "Invalid choice. Defaulting to Viewer role\n";
        }

        printSuccess("Logged in successfully!");
        return true;
    }
    else {
        printError("Invalid username or password! Please try again.");
        return false;
    }
}
void signUp(Graph& userGraph) {
    cout << "Welcome to Google Drive!\n";
    cout << "Please sign up to create an account.\n";
    string userName, password, question, answer;
    cout << "Enter username: ";
    cin >> userName;
    cout << "Enter password: ";
    cin >> password;
    cout << "Enter security question: ";
    cin.ignore();
    getline(cin, question);
    cout << "Enter answer to security question: ";
    getline(cin, answer);
    userGraph.signUp(userName, password, question, answer);
    printSuccess("User signed up successfully!");
}
void displayAuthMenu() {
    cout << "=====================================\n";
    cout << "||       Google Drive Login        ||\n";
    cout << "=====================================\n";
    cout << "|| 1. Sign Up                      ||\n";
    cout << "|| 2. Log In                       ||\n";
    cout << "|| 3. Forgot Password              ||\n";
    cout << "|| 4. Exit                         ||\n";
    cout << "=====================================\n";
    cout << "Enter your Choice: ";
}
void forgotPassword(Graph& userGraph) {
    clearScreen();
    cout << "Forgot Password\n";
    string userName, answer;
    cout << "Enter your username: ";
    cin >> userName;
    cout << "Enter the answer to your security question: ";
    cin.ignore();
    getline(cin, answer);
    userGraph.recoverPassword(userName, answer);
}
int main() {
    GoogleDrive drive;
    HashTable hashTable;
    Graph userGraph;
    string currentUser;
    UserRole currentRole = UserRole::VIEWER;
    bool isAuthenticated = false;
    DictionaryCompressor compressor;
    BackgroundSync syncManager;
    while (!isAuthenticated) {
        clearScreen();
        displayAuthMenu();
        int authChoice;
        cin >> authChoice;
        switch (authChoice) {
        case 1: 
            signUp(userGraph);
            break;
        case 2: 
            isAuthenticated = logIn(userGraph, currentUser, currentRole);
            if (!isAuthenticated) {
                printPrompt("Press Enter to try again...");
                cin.ignore();
                cin.get();
            }
            break;
        case 3: 
            forgotPassword(userGraph);
            printPrompt("Press Enter to return to the menu...");
            cin.ignore();
            cin.get();
            break;
        case 4: 
            cout << "Exiting...\n";
            return 0;
        default:
            printError("Invalid choice! Please try again.");
        }
    }
    int choice;
    bool exitProgram = false;
    do {
        clearScreen();
        displayMenu();
        choice = getValidatedChoice();
        switch (choice) {
        case 1: { 
            if (!hasWriteAccess(currentRole)) {
                printError("Access denied! Only Administrators and Editors can add folders.");
                cin.ignore();
                cin.get();
                break;
            }
            bool goBack = false;
            do {
                clearScreen();
                string parentName, folderName;
                cout << "Enter parent folder name: ";
                cin >> parentName;
                cout << "Enter folder name: ";
                cin >> folderName;
                drive.addChild(parentName, folderName, true);
                printPrompt("Do you want to go back to the main menu? (y/n): ");
                char backChoice;
                cin >> backChoice;
                if (backChoice == 'y' || backChoice == 'Y') goBack = true;
                else if (backChoice == 'n' || backChoice == 'N') goBack = false;
            } while (!goBack);
            break;
        }
        case 2: { 
            if (!hasWriteAccess(currentRole)) {
                printError("Access denied! Only Administrators and Editors can add files.");
                cin.ignore();
                cin.get();
                break;
            }
            bool goBack = false;
            do {
                clearScreen();
                string parentName, fileName, fileType, fileDate, fileOwner;
                int fileSize;
                cout << "Enter parent folder name: ";
                cin >> parentName;
                cout << "Enter file name: ";
                cin >> fileName;
                cout << "Enter file type: ";
                cin >> fileType;
                cout << "Enter file size (in KB): ";
                cin >> fileSize;
                cout << "Enter file creation date: ";
                cin >> fileDate;
                cout << "Enter file owner: ";
                cin >> fileOwner;
                drive.addChild(parentName, fileName, false);
                hashTable.insert(fileName, fileType, fileSize, fileDate, fileOwner);
                printSuccess("File added successfully!");
                printPrompt("Do you want to go back to the main menu? (y/n): ");
                char backChoice;
                cin >> backChoice;
                if (backChoice == 'y' || backChoice == 'Y') goBack = true;
                else if (backChoice == 'n' || backChoice == 'N') goBack = false;
            } while (!goBack);
            break;
        }
        case 3: { 
            if (!hasWriteAccess(currentRole)) {
                printError("Access denied! Only Administrators and Editors can rename files.");
                cin.ignore();
                cin.get();
                break;
            }
            bool goBack = false;
            do {
                clearScreen();
                string oldName, newName;
                cout << "Enter current name: ";
                cin >> oldName;
                cout << "Enter new name: ";
                cin >> newName;
                Node* node = drive.searchNode(oldName);
                if (node) {
                    try {
                        if (!node->isFolder) {
                            FileMetadata* metadata = hashTable.search(oldName);
                            if (metadata) {
                                hashTable.remove(oldName);
                                hashTable.insert(newName, metadata->type, metadata->size,
                                    metadata->date, metadata->owner);
                            }
                        }
                        if (drive.renameNode(oldName, newName)) {
                            printSuccess("Successfully renamed '" + oldName + "' to '" + newName + "'");
                        }
                        else {
                            printError("Failed to rename. Please try again.");
                        }
                    }
                    catch (const exception& e) {
                        printError("Error during rename: " + string(e.what()));
                        if (!node->isFolder) {
                            FileMetadata* metadata = hashTable.search(newName);
                            if (metadata) {
                                hashTable.remove(newName);
                                hashTable.insert(oldName, metadata->type, metadata->size,
                                    metadata->date, metadata->owner);
                            }
                        }
                    }
                }
                else {
                    printError("File or folder '" + oldName + "' not found!");
                }
                printPrompt("Do you want to go back to the main menu? (y/n): ");
                char backChoice;
                cin >> backChoice;
                if (backChoice == 'y' || backChoice == 'Y') goBack = true;
                else if (backChoice == 'n' || backChoice == 'N') goBack = false;
            } while (!goBack);
            break;
        }
        case 4: {
            if (!hasWriteAccess(currentRole)) {
                printError("Access denied! Only Administrators and Editors can delete files.");
                cin.ignore();
                cin.get();
                break;
            }
            bool goBack = false;
            do {
                clearScreen();
                string name;
                cout << "Enter name to delete: ";
                cin >> name;
                Node* node = drive.searchNode(name);
                if (node) {
                    if (!node->isFolder) {
                        hashTable.remove(name);
                    }
                    if (confirmAction("delete this file")) {
                        drive.deleteNode(name);
                        printSuccess("File deleted successfully!");
                    }
                    else {
                        printPrompt("Action canceled.");
                    }
                }
                else {
                    printError("File not found!");
                }
                printPrompt("Do you want to go back to the main menu? (y/n): ");
                char backChoice;
                cin >> backChoice;
                if (backChoice == 'y' || backChoice == 'Y') goBack = true;
                else if (backChoice == 'n' || backChoice == 'N') goBack = false;
            } while (!goBack);
            break;
        }
        case 5: { 
            bool goBack = false;
            do {
                clearScreen();
                std::string fileName;
                std::cout << "Enter the file name to search: ";
                std::cin >> fileName;
                drive.searchFile(fileName);
                printPrompt("Do you want to go back to the main menu? (y/n): ");
                char backChoice;
                cin >> backChoice;
                if (backChoice == 'y' || backChoice == 'Y') break;
                else if (backChoice == 'n' || backChoice == 'N') goBack = false;
            } while (!goBack);
            break;
        }
        case 6: { 
            bool goBack = false;
            do {
                clearScreen();
                displayTreeWithSymbols(drive.getRoot());
                printPrompt("Do you want to go back to the main menu? (y/n): ");
                char backChoice;
                cin >> backChoice;
                if (backChoice == 'y' || backChoice == 'Y') goBack = true;
                else if (backChoice == 'n' || backChoice == 'N') goBack = false;
            } while (!goBack);
            break;
        }
        case 7: { 
            if (drive.getRecycleBin().isEmpty()) {
                printError("Recycle Bin is empty.");
            }
            else {
                FileMetadata* file = drive.getRecycleBin().peek();
                cout << "Most recently deleted file: " << file->name << endl;
            }
            printPrompt("Press Enter to return to the main menu...");
            cin.ignore();
            cin.get();
            break;
        }
        case 8: { 
            if (!hasWriteAccess(currentRole)) {
                printError("Access denied! Only Administrators and Editors can restore files.");
                cin.ignore();
                cin.get();
                break;
            }
            drive.restoreFile();
            printPrompt("Press Enter to return to the main menu...");
            cin.ignore();
            cin.get();
            break;
        }
        case 9: {
            showHelp();
            break;
        }
        case 10: {
            clearScreen();
            if (drive.getRecentFiles().isEmpty()) {
                printError("No recent files to display.");
            }
            else {
                cout << "Recently accessed files:\n";
                drive.getRecentFiles().display();
            }
            printPrompt("Press Enter to return to the main menu...");
            cin.ignore();
            cin.get();
            break;
        }
        case 11: { 
            if (!hasWriteAccess(currentRole)) {
                printError("Access denied! Only Administrators and Editors can add file versions.");
                cin.ignore();
                cin.get();
                break;
            }
            clearScreen();
            string fileName, content;
            try {
                cout << "Enter the file name to add a version: ";
                cin >> fileName;
                if (fileName.empty()) {
                    throw runtime_error("File name cannot be empty!");
                }
                FileMetadata* metadata = hashTable.search(fileName);
                if (!metadata) {
                    throw runtime_error("File not found!");
                }
                if (metadata->owner != currentUser) {
                    bool hasAccess = false;
                    userGraph.bfsTraversal(currentUser, fileName);
                    if (!hasAccess) {
                        throw runtime_error("Access denied! You don't have permission to modify this file.");
                    }
                }
                cout << "Enter the content for the new version: ";
                cin.ignore();
                getline(cin, content);
                if (content.empty()) {
                    throw runtime_error("Version content cannot be empty!");
                }
                metadata->versionHistory.addVersion(content);
                printSuccess("New version added successfully!");

            }
            catch (const exception& e) {
                printError(e.what());
            }
            printPrompt("Press Enter to return to the main menu...");
            cin.ignore();
            cin.get();
            break;
        }
        case 12: {
            clearScreen();
            string fileName;
            int versionNumber;
            try {
                cout << "Enter the file name to rollback: ";
                cin >> fileName;
                if (fileName.empty()) {
                    throw runtime_error("File name cannot be empty!");
                }
                FileMetadata* metadata = hashTable.search(fileName);
                if (!metadata) {
                    throw runtime_error("File not found!");
                }
                if (metadata->owner != currentUser) {
                    bool hasAccess = false;
                    userGraph.bfsTraversal(currentUser, fileName);
                    if (!hasAccess) {
                        throw runtime_error("Access denied! You don't have permission to modify this file.");
                    }
                }
                cout << "\nAvailable versions:\n";
                metadata->versionHistory.displayHistory();
                cout << "\nEnter the version number to rollback to: ";
                if (!(cin >> versionNumber)) {
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    throw runtime_error("Invalid version number!");
                }
                metadata->versionHistory.rollbackToVersion(versionNumber);
                printSuccess("File rolled back to version " + to_string(versionNumber) + " successfully!");
            }
            catch (const exception& e) {
                printError(e.what());
            }
            printPrompt("Press Enter to return to the main menu...");
            cin.ignore();
            cin.get();
            break;
        }
        case 13: {
            clearScreen();
            string fileName;
            try {
                cout << "Enter the file name to view version history: ";
                cin >> fileName;
                if (fileName.empty()) {
                    throw runtime_error("File name cannot be empty!");
                }
                FileMetadata* metadata = hashTable.search(fileName);
                if (!metadata) {
                    throw runtime_error("File not found!");
                }
                if (metadata->owner != currentUser) {
                    bool hasAccess = false;
                    userGraph.bfsTraversal(currentUser, fileName);
                    if (!hasAccess) {
                        throw runtime_error("Access denied! You don't have permission to view this file's history.");
                    }
                }
                cout << "\nVersion history for file: " << fileName << endl;
                cout << "Owner: " << metadata->owner << endl;
                cout << "Last Modified: " << metadata->date << endl;
                cout << "\nVersions:\n";
                metadata->versionHistory.displayHistory();
            }
            catch (const exception& e) {
                printError(e.what());
            }
            printPrompt("Press Enter to return to the main menu...");
            cin.ignore();
            cin.get();
            break;
        }
        case 14:
            cout << "Exiting...\n";
            exitProgram = true;
            break;
        case 15: {
            if (!hasWriteAccess(currentRole)) {
                printError("Access denied! Only Administrators and Editors can create files.");
                cin.ignore();
                cin.get();
                break;
            }
            clearScreen();
            string parentName, fileName, fileType, fileDate, fileOwner;
            int fileSize;
            cout << "Enter parent folder name: ";
            cin >> parentName;
            cout << "Enter file name: ";
            cin >> fileName;
            cout << "Enter file type: ";
            cin >> fileType;
            cout << "Enter file size (in KB): ";
            cin >> fileSize;
            cout << "Enter file creation date: ";
            cin >> fileDate;
            cout << "Enter file owner: ";
            cin >> fileOwner;
            drive.addChild(parentName, fileName, false);
            hashTable.insert(fileName, fileType, fileSize, fileDate, fileOwner);
            printSuccess("File created successfully!");
            printPrompt("Press Enter to return to the main menu...");
            cin.ignore();
            cin.get();
            break;
        }
        case 16: {
            clearScreen();
            string fileName;
            cout << "Enter the file name to read: ";
            cin >> fileName;
            FileMetadata* metadata = hashTable.search(fileName);
            if (metadata) {
                cout << "File Details:\n";
                cout << "Name: " << metadata->name << "\n";
                cout << "Type: " << metadata->type << "\n";
                cout << "Size: " << metadata->size << " KB\n";
                cout << "Date: " << metadata->date << "\n";
                cout << "Owner: " << metadata->owner << "\n";
                cout << "Content: " << metadata->versionHistory.getCurrentVersionContent() << "\n";
            }
            else {
                printError("File not found!");
            }
            printPrompt("Press Enter to return to the main menu...");
            cin.ignore();
            cin.get();
            break;
        }
        case 17: { 
            if (!hasWriteAccess(currentRole)) {
                printError("Access denied! Only Administrators and Editors can update files.");
                cin.ignore();
                cin.get();
                break;
            }
            clearScreen();
            string fileName, newContent;
            cout << "Enter the file name to update: ";
            cin >> fileName;
            FileMetadata* metadata = hashTable.search(fileName);
            if (metadata) {
                cout << "Enter the new content for the file: ";
                cin.ignore();
                getline(cin, newContent);
                metadata->versionHistory.addVersion(metadata->versionHistory.getCurrentVersionContent());
                metadata->versionHistory.addVersion(newContent);
                printSuccess("File updated successfully!");
            }
            else {
                printError("File not found!");
            }
            printPrompt("Press Enter to return to the main menu...");
            cin.ignore();
            cin.get();
            break;
        }
        case 18: { 
            if (!hasWriteAccess(currentRole)) {
                printError("Access denied! Only Administrators and Editors can delete files.");
                cin.ignore();
                cin.get();
                break;
            }
            clearScreen();
            string fileName;
            cout << "Enter the file name to delete: ";
            cin >> fileName;
            FileMetadata* metadata = hashTable.search(fileName);
            if (metadata) {
                drive.deleteNode(fileName);
                drive.getRecycleBin().push(metadata);
                printSuccess("File deleted successfully and moved to Recycle Bin!");
            }
            else {
                printError("File not found!");
            }
            printPrompt("Press Enter to return to the main menu...");
            cin.ignore();
            cin.get();
            break;
        }
        case 19: {
            clearScreen();
            cout << "BFS Tree Traversal for File Access\n";
            cout << "===================================\n";
            string fileName;
            cout << "Enter file name to check access: ";
            cin >> fileName;
            FileMetadata* metadata = hashTable.search(fileName);
            if (metadata) {
                cout << "\nPerforming BFS traversal to find users with access to '" << fileName << "':\n";
                showLoading("Traversing");
                userGraph.bfsTraversal(currentUser, fileName);
            }
            else {
                printError("File not found in the system!");
            }
            printPrompt("Press Enter to return to the main menu...");
            cin.ignore();
            cin.get();
            break;
        }
        case 20: {
            clearScreen();
            cout << "DFS Tree Traversal for File Access\n";
            cout << "===================================\n";
            string fileName;
            cout << "Enter file name to check access: ";
            cin >> fileName;
            FileMetadata* metadata = hashTable.search(fileName);
            if (metadata) {
                cout << "\nPerforming DFS traversal to find users with access to '" << fileName << "':\n";
                showLoading("Traversing");
                userGraph.dfsTraversal(currentUser, fileName);
            }
            else {
                printError("File not found in the system!");
            }
            printPrompt("Press Enter to return to the main menu...");
            cin.ignore();
            cin.get();
            break;
        }
        case 21: { 
            if (currentRole != UserRole::ADMIN) {
                printError("Only administrators can change user roles!");
                break;
            }
            string username;
            int roleChoice;
            cout << "Enter username: ";
            cin >> username;
            cout << "Select role (1: Viewer, 2: Editor, 3: Admin): ";
            cin >> roleChoice;
            UserRole newRole;
            switch (roleChoice) {
            case 1: newRole = UserRole::VIEWER; break;
            case 2: newRole = UserRole::EDITOR; break;
            case 3: newRole = UserRole::ADMIN; break;
            default: newRole = UserRole::VIEWER;
            }
            printSuccess("User role updated successfully!");
            break;
        }
        case 22: {
            if (!hasWriteAccess(currentRole)) {
                printError("Access denied! Only Administrators and Editors can compress files.");
                cin.ignore();
                cin.get();
                break;
            }
            clearScreen();
            string fileName;
            cout << "Enter file name to compress: ";
            cin >> fileName;

            FileMetadata* metadata = hashTable.search(fileName);
            if (metadata) {
                string content = metadata->versionHistory.getCurrentVersionContent();
                string compressed = compressRLE(content);
                metadata->versionHistory.addVersion(compressed);
                printSuccess("File compressed successfully!");
                syncManager.queueSync(fileName, compressed);
            }
            else {
                printError("File not found!");
            }
            break;
        }
        case 23: { 
            if (!hasWriteAccess(currentRole)) {
                printError("Access denied! Only Administrators and Editors can decompress files.");
                cin.ignore();
                cin.get();
                break;
            }
            clearScreen();
            string fileName;
            cout << "Enter file name to decompress: ";
            cin >> fileName;
            FileMetadata* metadata = hashTable.search(fileName);
            if (metadata) {
                string content = metadata->versionHistory.getCurrentVersionContent();
                string decompressed = decompressRLE(content);
                metadata->versionHistory.addVersion(decompressed);
                printSuccess("File decompressed successfully!");
            }
            else {
                printError("File not found!");
            }
            break;
        }
        case 24: { 
            if (!hasWriteAccess(currentRole)) {
                printError("Access denied! Only Administrators and Editors can queue cloud sync.");
                cin.ignore();
                cin.get();
                break;
            }
            clearScreen();
            string fileName;
            cout << "Enter file name to sync: ";
            cin >> fileName;

            FileMetadata* metadata = hashTable.search(fileName);
            if (metadata) {
                string content = metadata->versionHistory.getCurrentVersionContent();
                syncManager.queueSync(fileName, content);
                printSuccess("File queued for synchronization!");
            }
            else {
                printError("File not found!");
            }
            break;
        }
        default:
            cout << "Invalid choice!\n";
        }
    } while (!exitProgram);
    return 0;
}