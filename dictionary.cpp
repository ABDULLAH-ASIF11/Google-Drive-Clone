#include "dictionary.h"
using namespace std;

int Dictionary::hash(const std::string& word) const {
    int hash = 0;
    for (char c : word) {
        hash = (hash * 31 + c) % tableSize;
    }
    return hash;
}
Dictionary::Dictionary(int size) : tableSize(size), nextCode(0) {
    table = new Entry * [size];
    for (int i = 0; i < size; i++) {
        table[i] = nullptr;
    }
}
Dictionary::~Dictionary() {
    for (int i = 0; i < tableSize; i++) {
        Entry* current = table[i];
        while (current) {
            Entry* next = current->next;
            delete current;
            current = next;
        }
    }
    delete[] table;
}
int Dictionary::add(const std::string& word) {
    int index = hash(word);
    Entry* current = table[index];
    while (current) {
        if (current->word == word) {
            return current->code;
        }
        current = current->next;
    }
    Entry* newEntry = new Entry(word, nextCode);
    newEntry->next = table[index];
    table[index] = newEntry;
    return nextCode++;
}
std::string Dictionary::getWord(int code) const {
    for (int i = 0; i < tableSize; i++) {
        Entry* current = table[i];
        while (current) {
            if (current->code == code) {
                return current->word;
            }
            current = current->next;
        }
    }
    return "";
}

DictionaryCompressor::DictionaryCompressor() {}
std::string DictionaryCompressor::compress(const std::string& input) {
    std::string result;
    std::string word;
    size_t start = 0;
    for (size_t i = 0; i <= input.length(); i++) {
        if (i == input.length() || input[i] == ' ') {
            if (start < i) {
                word = input.substr(start, i - start);
                result += std::to_string(dict.add(word)) + " ";
            }
            start = i + 1;
        }
    }
    return result;
}
std::string DictionaryCompressor::decompress(const std::string& input) {
    std::string result;
    std::string code;
    size_t start = 0;
    for (size_t i = 0; i <= input.length(); i++) {
        if (i == input.length() || input[i] == ' ') {
            if (start < i) {
                code = input.substr(start, i - start);
                try {
                    int codeNum = std::stoi(code);
                    result += dict.getWord(codeNum) + " ";
                }
                catch (const std::exception& e) {
                    result += "[InvalidCode] ";
                }
            }
            start = i + 1;
        }
    }
    if (!result.empty() && result.back() == ' ') {
        result.pop_back();
    }
    return result;
}

BackgroundSync::BackgroundSync() : front(0), rear(-1), size(0), running(true) {
    queue = new SyncItem[MAX_QUEUE_SIZE];
}
BackgroundSync::~BackgroundSync() {
    running = false;
    delete[] queue;
}
void BackgroundSync::queueSync(const string& filename, const string& content) {
    if (size < MAX_QUEUE_SIZE) {
        rear = (rear + 1) % MAX_QUEUE_SIZE;
        queue[rear].filename = filename;
        queue[rear].content = content;
        size++;
        cout << "Queued " << filename << " for synchronization" << endl;
        processSyncQueue();
    }
}
void BackgroundSync::processSyncQueue() {
    while (size > 0) {
        string filename = queue[front].filename;
        cout << "Syncing " << filename << " to cloud..." << endl;
        this_thread::sleep_for(chrono::seconds(3));
        cout << "Sync complete for " << filename << endl;
        front = (front + 1) % MAX_QUEUE_SIZE;
        size--;
    }
}