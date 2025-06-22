#pragma once
#ifndef GOOGLEDRIVE_H
#define GOOGLEDRIVE_H

#include "hashTable.h"
#include <iostream>
using namespace std;

FileMetadata::FileMetadata(std::string name, std::string type, int size, std::string date, std::string owner) {
    this->name = name;
    this->type = type;
    this->size = size;
    this->date = date;
    this->owner = owner;
    this->next = nullptr;
}

int hashFunction(const string& key, int tableSize) {
    int hash = 0;
    for (char ch : key)
        hash = (hash * 31 + ch) % tableSize;
    return hash;
}

HashTable::HashTable() : capacity(TABLE_SIZE) {
    table = new FileMetadata * [TABLE_SIZE];
    for (int i = 0; i < TABLE_SIZE; ++i)
        table[i] = nullptr;
}

HashTable::~HashTable() {
    for (int i = 0; i < TABLE_SIZE; ++i) {
        FileMetadata* entry = table[i];
        while (entry) {
            FileMetadata* temp = entry;
            entry = entry->next;
            delete temp;
        }
    }
    delete[] table;
}

void HashTable::insert(const std::string& name, const std::string& type,
    int size, const std::string& date, const std::string& owner) {
    int index = hashFunction(name, TABLE_SIZE);
    FileMetadata* newEntry = new FileMetadata(name, type, size, date, owner);
    newEntry->next = table[index];
    table[index] = newEntry;
}

FileMetadata* HashTable::search(const std::string& name) {
    int index = hashFunction(name, TABLE_SIZE);
    FileMetadata* entry = table[index];
    while (entry) {
        if (entry->name == name)
            return entry;
        entry = entry->next;
    }
    return nullptr;
}

void HashTable::remove(const std::string& name) {
    int index = hashFunction(name, TABLE_SIZE);
    FileMetadata* entry = table[index];
    FileMetadata* prev = nullptr;
    while (entry) {
        if (entry->name == name) {
            if (prev) prev->next = entry->next;
            else table[index] = entry->next;
            delete entry;
            return;
        }
        prev = entry;
        entry = entry->next;
    }
    std::cout << "File '" << name << "' not found." << endl;
}
#endif