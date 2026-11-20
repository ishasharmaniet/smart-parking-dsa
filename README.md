# Smart Parking Allocation and Reservation Platform 🚗⚡

[![NIET Greater Noida](https://img.shields.io/badge/Institute-NIET%20Greater%20Noida-003366.svg)](https://www.niet.co.in/)
[![Course](https://img.shields.io/badge/Course-DSA--II%20(CCSE0301)-blue.svg)]()
[![SDG 11](https://img.shields.io/badge/UN%20SDG-11%20Sustainable%20Cities-emerald.svg)](https://sdgs.un.org/goals/goal11)
[![Status](https://img.shields.io/badge/PBL%20Milestone-Review%203%20(100%25)-success.svg)]()
[![License](https://img.shields.io/badge/License-MIT-lightgrey.svg)]()

> **Project Based Learning (PBL) Final Capstone Report & Demonstration**  
> **Department of Computer Science & Engineering**, Noida Institute of Engineering & Technology (NIET), Greater Noida.

---

## 📌 Project Overview
The **Smart Parking Allocation and Reservation Platform** is a unified algorithmic system designed to solve urban parking congestion, reduce vehicular cruising emissions, and optimize multi-level parking assets. Aligned with **UN Sustainable Development Goal 11 (Sustainable Cities & Communities)**, this project demonstrates the end-to-end practical application of all 5 units of the **Data Structures and Algorithms-II (CCSE0301)** curriculum.

---

## 🚀 Live Interactive Web Visualizer
🌐 **Live GitHub Pages URL:** `https://ishasharmaniet.github.io/smart-parking-dsa/`  
*(Features real-time 2D parking lot visualizer, live greedy allocation, dynamic programming timeline scheduler, backtracking solver, and KMP license plate search).*

---

## 🧠 Core DSA-II Concepts Applied

| Unit / Module | Syllabus Topic | Smart Parking Platform Application | Theoretical Complexity |
| :--- | :--- | :--- | :--- |
| **Unit I** | **Divide-and-Conquer** | **Merge Sort:** Multi-level slot proximity distance ranking from entry barrier.<br>**Binary Search:** Fast logarithmic retrieval by unique Slot ID. | Merge Sort: $\Theta(N \log N)$<br>Binary Search: $O(\log N)$ |
| **Unit II** | **Greedy Algorithms** | **Greedy Nearest-Fit Allocation:** Real-time gate vehicle ingress minimizing driver walking distance while satisfying size, EV, and handicap criteria. | Time: $O(N)$<br>Space: $O(1)$ |
| **Unit III** | **Dynamic Programming** | **Weighted Interval Scheduling:** Resolves conflicting VIP advance bookings over overlapping hours to maximize total facility revenue ($OPT(j)$ Bellman recurrence). | Time: $O(M \log M)$<br>Space: $O(M)$ |
| **Unit IV** | **Backtracking** | **Constrained Multi-Attribute Batch Assignment:** Recursively explores state-space tree with pruning to allocate concurrent vehicle batches under multi-dimensional constraints. | Time: $O(S^V)$ (pruned)<br>Space: $O(V)$ |
| **Unit V** | **String Matching & NP Analysis** | **Knuth-Morris-Pratt (KMP):** $O(n+m)$ linear-time partial license plate pattern matching without text pointer backtracking.<br>**Complexity:** 2D Bin Packing NP-Hardness reduction. | Preprocessing: $O(m)$<br>Matching: $O(n)$ |

---

## 📂 Repository Contents

```text
├── index.html                                        # Live Interactive Web Visualizer (GitHub Pages)
├── smart_parking.c                                   # Complete ANSI C Core Program
├── NIET_PBL_Final_Report_CCSE0301_Smart_Parking.docx # Official College Master Final Report (.DOCX)
├── run_simulation.py                                 # Automated Python Test Harness
├── simulation_output.txt                             # Verified Terminal Benchmark Output
└── README.md                                         # Project Documentation & Guide
```

---

## 🛠️ How to Compile & Run the C Program Locally

```bash
# Clone the repository
git clone https://github.com/ishasharmaniet/smart-parking-dsa.git
cd smart-parking-dsa

# Compile with GCC
gcc -Wall -O2 smart_parking.c -o smart_parking

# Run the program
./smart_parking
```

---

## 👥 Student & Faculty Details
- **Student Name:** Isha Sharma
- **Roll No / ERP ID:** 2501330120091
- **Program & Branch:** B.Tech Computer Science & Engineering (BTech - CS-B)
- **Faculty Mentor:** Ms. Pooja Sharma (Assistant Professor, Dept. of CSE)
- **Institution:** Noida Institute of Engineering & Technology, Greater Noida
- **Session:** Academic Year 2026-2027

