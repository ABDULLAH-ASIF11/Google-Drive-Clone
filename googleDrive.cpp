#include "googleDrive.h"
#include <iostream>
#include "stack.h"
using namespace std;

Node::Node(const string& name, bool isFolder) {
    this->name = name;
    this->isFolder = isFolder;
    this->firstChild = nullptr;
    this->nextSibling = nullptr;
}
GoogleDrive::GoogleDrive() : recentFiles(5) { 
    root = new Node("Root", true);
}
GoogleDrive::~GoogleDrive() {
    deleteTree(root);
}
void GoogleDrive::deleteTree(Node* node) {
    if (!node) return;
    deleteTree(node->firstChild);
    deleteTree(node->nextSibling);
    delete node;
}
Node* GoogleDrive::getRoot() {
    return root;
}
void GoogleDrive::addChild(const string& parentName, const string& name, bool isFolder) {
    Node* parent = searchNode(parentName);
    if (!parent || !parent->isFolder) {
        cout << "Parent folder '" << parentName << "' not found or is not a folder!" << endl;
        return;
    }
    Node* newNode = new Node(name, isFolder);
    newNode->firstChild = nullptr;
    newNode->nextSibling = nullptr;
    if (!parent->firstChild) {
        parent->firstChild = newNode;
    }
    else {
        Node* temp = parent->firstChild;
        while (temp->nextSibling) {
            temp = temp->nextSibling;
        }
        temp->nextSibling = newNode;
    }
}
void GoogleDrive::displayTree() {
    displayTreeHelper(root, 0);
}
void GoogleDrive::displayTreeHelper(Node* node, int depth) {
    if (!node) return;
    for (int i = 0; i < depth; ++i) cout << "  ";
    cout << (node->isFolder ? "[Folder] " : "[File] ") << node->name << endl;
    displayTreeHelper(node->firstChild, depth + 1);
    displayTreeHelper(node->nextSibling, depth);
}
Node* GoogleDrive::searchNodeHelper(Node* node, const string& name) {
    if (!node) return nullptr;
    if (node->name == name) return node;
    Node* found = searchNodeHelper(node->firstChild, name);
    if (found) return found;
    return searchNodeHelper(node->nextSibling, name);
}
void GoogleDrive::deleteNode(const std::string& name) {
    if (root->name == name) {
        cout << "Cannot delete the root folder!" << endl;
        return;
    }
    Node* parent = nullptr;
    Node* current = root;
    queue<Node*> q;
    q.push(root);
    while (!q.empty()) {
        Node* temp = q.front();
        q.pop();
        Node* child = temp->firstChild;
        while (child) {
            if (child->name == name) {
                parent = temp;
                current = child;
                break;
            }
            q.push(child);
            child = child->nextSibling;
        }
        if (parent) break;
    }
    if (!parent) {
        cout << "Node '" << name << "' not found!" << endl;
        return;
    }
    if (!current->isFolder) {
        recycleBin.push(new FileMetadata(current->name, "file", 0, "unknown", "unknown", parent->name));
    }
    if (parent->firstChild == current) {
        parent->firstChild = current->nextSibling;
    }
    else {
        Node* sibling = parent->firstChild;
        while (sibling->nextSibling != current) {
            sibling = sibling->nextSibling;
        }
        sibling->nextSibling = current->nextSibling;
    }
    deleteTree(current);
}
void GoogleDrive::restoreFile() {
    if (recycleBin.isEmpty()) {
        cout << "Recycle Bin is empty. No files to restore." << endl;
        return;
    }
    FileMetadata* file = recycleBin.pop();
    if (!file) return;
    addChild(file->parentFolder, file->name, false);
    cout << "File '" << file->name << "' has been restored to folder '" << file->parentFolder << "'." << endl;
    delete file;
}
Stack& GoogleDrive::getRecycleBin() {
    return recycleBin;
}
Queue& GoogleDrive::getRecentFiles() {
    return recentFiles;
}
Node* GoogleDrive::searchNode(const std::string& name) {
    Node* result = searchNodeHelper(root, name);
    if (result && !result->isFolder) {
        recentFiles.enqueue(new FileMetadata(result->name, "file", 0, "unknown", "unknown", "unknown"));
    }
    return result;
}
Graph& GoogleDrive::getUserGraph() {
    return userGraph;
}
void GoogleDrive::searchFile(const std::string& fileName) {
    FileMetadata* metadata = hashTable.search(fileName);
    if (metadata) {
        cout << "File found in hash table:\n";
        cout << "Name: " << metadata->name << "\n";
        cout << "Type: " << metadata->type << "\n";
        cout << "Size: " << metadata->size << " KB\n";
        cout << "Date: " << metadata->date << "\n";
        cout << "Owner: " << metadata->owner << "\n";
        return;
    }
    Node* foundNode = searchNodeHelper(root, fileName);
    if (foundNode) {
        cout << "File found in directory tree:\n";
        cout << "Name: " << foundNode->name << "\n";
        cout << "Type: " << (foundNode->isFolder ? "Folder" : "File") << "\n";
    }
    else {
        cout << "File not found.\n";
    }
}
bool GoogleDrive::renameNode(const string& oldName, const string& newName) {
    Node* node = searchNode(oldName);
    if (!node) return false;
    if (searchNode(newName)) return false;
    node->name = newName;
    return true;
}