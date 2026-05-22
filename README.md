# 📁 VIRTUAL FILE ARCHITECTURE (GOOGLE DRIVE CLONE)

A high-performance, terminal-based cloud storage simulation engineered entirely in **C++**. This architecture maps complex file hierarchies and directory mechanics natively using low-level **Data Structures & Algorithms (DSA)** instead of an external database engine.

---

> ### 🛑 TECHNICAL ARCHITECTURE OVERVIEW
> * **N-ary Tree Topology:** Simulates infinite directory nesting. Each folder is a structural node retaining pointers to its parent scope and an expansive collection of subdirectories.
> * **Linear File Streams:** Individual file metadata sets are stored dynamically inside isolated, folder-bound Linked Lists.
> * **Pointer-Driven Space Navigation:** File path manipulation (`cd`, `mkdir`) updates working pointer memory addresses directly, bypassing heavy runtime overhead.

---

## ⚡ OPERATIONAL SPECIFICATIONS

### 1️⃣ Directory Control & Workspace Traversals
* **`mkdir` Pattern:** Allocates a dynamic tree node inside the active scope context.
* **`cd` Vector shifts:** Moves the active system memory address forward to a child node or backward to the parent pointer (`cd ..`).
* **`ls` Structural Scan:** Linearly parses through both child folder pointers and file linked lists to print active system layouts.

### 2️⃣ Low-Level Content Modification
* **File Instantiation:** Dynamically appends a new file block directly into the local scope's structural linked list.
* **`rm` Memory Deallocation:** Safeguards memory health by performing clean pointer prunings on isolated files or recursive branch teardowns for complete directories.
* **System Metadata Logs:** Tracks real-time attributes including custom filenames, file extension strings, and calculated sizing.

### 3️⃣ Cloud Query Utilities
* **Recursive Global Search:** Traverses down from the root system node using algorithmic path tracking to find target files instantly.
* **Destructor Chains:** Explicit deep-cleaning memory management routines eliminate any potential runtime memory leaks.

---

## 📊 REPOSITORY LAYOUT

* 📂 **include/**
  * 📄 `DriveManager.h` — Structural definitions for data nodes and operational prototypes.
* 📂 **src/**
  * 📄 `DriveManager.cpp` — Core traversal algorithms, pointer shifts, and low-level system actions.
* 📄 `main.cpp` — Operational driver running the standard console UI input loop.

---

## 🛠️ BUILD ENGINE INSTRUCTIONS

| Phase | Command Line Interface | Output |
| :--- | :--- | :--- |
| **1. Compilation** | `g++ src/DriveManager.cpp main.cpp -I include -o DriveClone` | Generates low-level executable file system binary |
| **2. Execution** | `./DriveClone` | Launches the interactive terminal control interface |

---

### 👨‍💻 THE CREATOR

| ABDULLAH ASIF |
| :---: |
| Full-Stack / Systems Developer |
| [![LinkedIn](https://img.shields.io/badge/LINKEDIN-%230077B5?style=for-the-badge&logo=linkedin&logoColor=white)](https://www.linkedin.com/in/abdullah-asif-bhatti/) [![GitHub](https://img.shields.io/badge/GITHUB-%23100000?style=for-the-badge&logo=github&logoColor=white)](https://github.com/ABDULLAH-ASIF11) |
