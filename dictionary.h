#pragma once
#include "googleDrive.h"
#include "hashTable.h"
#include "graph.h"
#include "dictionary.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <queue>
#include <string>
using namespace std;

class Dictionary {
private:
    struct Entry {
        std::string word;
        int code;
        Entry* next;
        Entry(const std::string& w, int c) : word(w), code(c), next(nullptr) {}
    };
    Entry** table;
    int tableSize;
    int nextCode;
    int hash(const std::string& word) const;
public:
    Dictionary(int size = 1000);
    ~Dictionary();
    int add(const std::string& word);
    std::string getWord(int code) const;
};
class DictionaryCompressor {
private:
    Dictionary dict;
public:
    DictionaryCompressor();
    std::string compress(const std::string& input);
    std::string decompress(const std::string& input);
};
class BackgroundSync {
private:
    struct SyncItem {
        string filename;
        string content;
    };
    static const int MAX_QUEUE_SIZE = 1000;
    SyncItem* queue;
    int front;
    int rear;
    int size;
    bool running;
    void processSyncQueue();
public:
    BackgroundSync();
    ~BackgroundSync();
    void queueSync(const string& filename, const string& content);
};