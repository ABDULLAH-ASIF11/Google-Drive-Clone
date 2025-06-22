#pragma once
#include <ctime>
#include "HashTable.h"
struct StackNode {
    FileMetadata* file;
    time_t deletionTime;
    StackNode* next;
    StackNode(FileMetadata* file) : file(file), deletionTime(time(nullptr)), next(nullptr) {}
};
class Stack {
private:
    StackNode* top;
    int autoDeleteThreshold;
public:
    Stack(int threshold = 3600) : top(nullptr), autoDeleteThreshold(threshold) {}

    ~Stack() {
        while (top) {
            StackNode* temp = top;
            top = top->next;
            delete temp->file;
            delete temp;
        }
    }
    void push(FileMetadata* file) {
        autoDeleteOldEntries(); 
        StackNode* newNode = new StackNode(file);
        newNode->next = top;
        top = newNode;
    }
    FileMetadata* pop() {
        if (!top) return nullptr;
        StackNode* temp = top;
        top = top->next;
        FileMetadata* file = temp->file;
        delete temp;
        return file;
    }
    FileMetadata* peek() const {
        return top ? top->file : nullptr;
    }
    bool isEmpty() const {
        return top == nullptr;
    }
    void autoDeleteOldEntries() {
        time_t currentTime = time(nullptr);
        while (top && difftime(currentTime, top->deletionTime) > autoDeleteThreshold) {
            StackNode* temp = top;
            top = top->next;
            delete temp->file;
            delete temp;
        }
    }
};