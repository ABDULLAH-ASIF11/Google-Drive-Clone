#pragma once
#ifndef GOOGLEDRIVE_H
#define GOOGLEDRIVE_H
#include <string>
#include <queue>
#include "queue.h"
#include "stack.h"
#include "graph.h"
#include "doublyLinkedList.h"
#include "hashTable.h"
using namespace std;

struct Node {
    std::string name;
    bool isFolder;
    Node* firstChild;
    Node* nextSibling;
    Node(const string& name, bool isFolder);
};

class GoogleDrive {
private:
    Node* root;
    Stack recycleBin; 
    Queue recentFiles; 
    Graph userGraph; 
    HashTable hashTable;
    void displayTreeHelper(Node* node, int depth);
    void deleteTree(Node* node);
    Node* searchNodeHelper(Node* node, const string& name);

public:
    GoogleDrive();              
    ~GoogleDrive();           
    void addChild(const string& parentName, const string& name, bool isFolder);
    void displayTree();
    Node* searchNode(const string& name);
    void deleteNode(const string& name);
    void restoreFile();
    Node* getRoot();
    Stack& getRecycleBin();
    Queue& getRecentFiles();
    Graph& getUserGraph();
    void searchFile(const string& fileName);
    bool renameNode(const string& oldName, const string& newName);
};
#endif