#pragma once
#include <string>
#include <iostream>
using namespace std;

struct VersionNode {
    int versionNumber;     
    std::string content;        
    VersionNode* prev;       
    VersionNode* next;   

    VersionNode(int versionNumber, const std::string& content)
        : versionNumber(versionNumber), content(content), prev(nullptr), next(nullptr) {
    }
};
class DoublyLinkedList {
private:
    VersionNode* head;       
    VersionNode* tail;         
    int versionCount;    
public:
    DoublyLinkedList() : head(nullptr), tail(nullptr), versionCount(0) {}
    ~DoublyLinkedList() {
        while (head) {
            VersionNode* temp = head;
            head = head->next;
            delete temp;
        }
    }
    void addVersion(const std::string& content) {
        versionCount++;
        VersionNode* newNode = new VersionNode(versionCount, content);
        if (!head) {
            head = tail = newNode;
        }
        else {
            tail->next = newNode;
            newNode->prev = tail;
            tail = newNode;
        }
    }
    void rollbackToVersion(int versionNumber) {
        VersionNode* current = tail;
        while (current && current->versionNumber != versionNumber) {
            VersionNode* temp = current;
            current = current->prev;
            delete temp; 
        }
        if (current) {
            current->next = nullptr;
            tail = current;
            versionCount = versionNumber;
        }
        else {
            cout << "Version " << versionNumber << " not found." << endl;
        }
    }
    void displayHistory() const {
        VersionNode* current = head;
        while (current) {
            cout << "Version " << current->versionNumber << ": " << current->content << endl;
            current = current->next;
        }
    }
    string getCurrentVersionContent() const {
        return tail ? tail->content : "No versions available.";
    }
};