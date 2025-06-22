#pragma once
#include "Stack.h" 
#include<iostream>
using namespace std;
struct QueueNode {
    FileMetadata* file; 
    QueueNode* next;  

    QueueNode(FileMetadata* file) : file(file), next(nullptr) {}
};
class Queue {
private:
    QueueNode* front; 
    QueueNode* rear; 
    int size;        
    int capacity; 

public:
    Queue(int capacity) : front(nullptr), rear(nullptr), size(0), capacity(capacity) {}
    ~Queue() {
        while (front) {
            QueueNode* temp = front;
            front = front->next;
            delete temp->file; 
            delete temp;    
        }
    }
    void enqueue(FileMetadata* file) {
        cout << "Enqueuing file: " << file->name << endl;
        if (size == capacity) {
            cout << "Queue is full. Dequeuing oldest file." << endl;
            dequeue();
        }
        QueueNode* newNode = new QueueNode(file);
        if (!rear) {
            front = rear = newNode;
        }
        else {
            rear->next = newNode;
            rear = newNode;
        }
        size++;
    }
    void dequeue() {
        if (!front) return; 

        QueueNode* temp = front;
        front = front->next;
        if (!front) rear = nullptr; 
        delete temp->file;          
        delete temp;           
        size--;
    }
    void display() const {
        QueueNode* current = front;
        if (!current) {
            cout << "Queue is empty." << endl;
            return;
        }
        while (current) {
            cout << "File: " << current->file->name << ", Type: " << current->file->type << endl;
            current = current->next;
        }
    }
    bool isEmpty() const {
        return size == 0;
    }
};
