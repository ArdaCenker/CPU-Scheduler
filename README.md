# Advanced CPU Scheduler Simulation

## 📝 About The Project
This repository contains a system-level **CPU Scheduling Simulation** written in C. Unlike standard First-Come-First-Serve or static Round-Robin schedulers, this project implements an advanced, dynamic priority-based scheduling algorithm.

The core of the scheduler utilizes a **Binomial Heap** data structure to efficiently manage the process queue. The simulation evaluates different Time Quantum (`q`) values and automatically determines the optimal quantum that results in the **Minimum Average Waiting Time (AWT)**.

## ⚙️ Key Features & Implementation Details
* **Binomial Heap Queue:** Efficient insertion, extraction, and merging of processes using a custom-built Binomial Heap data structure.
* **Dynamic Priority Calculation:** Process priorities are not static. They are mathematically recalculated during execution based on remaining execution time and the maximum execution time (`e_max`) in the batch.
* **Quantum Optimization Engine:** The system iterates through potential Time Quantum values (from 1 to `e_max + 2`) to benchmark performance and pinpoint the best `q` value.
* **Detailed Context Switch Logging:** Simulates realistic CPU context-switch delays (0.1 units) and provides a detailed step-by-step execution trace.

## 💻 Built With
* **Language:** C
* **Core Concepts:** Data Structures (Binomial Heaps), Dynamic Scheduling Algorithms, System-Level Simulation, Performance Benchmarking.

## 🚀 Getting Started

### Prerequisites
* A C compiler (e.g., `gcc`).

### Installation & Execution

1. **Clone the repository:**
   ```bash
   git clone [https://github.com/ArdaCenker/CPU-Scheduler.git](https://github.com/ArdaCenker/CPU-Scheduler.git)

   ```

2. **Navigate to the project directory:**
```bash
cd CPU-Scheduler

```


3. **Compile the C program:**
```bash
gcc 150123012_prj2.c -o scheduler -lm

```


*(Note: The `-lm` flag is required to link the math library used for exponential and power calculations in the dynamic priority formula).*
4. **Prepare the Input File:**
The program reads process data from a file named `input.txt`. Place this file in the same directory as the executable.
**Input Format:** Each line should represent a process formatted as `ExecutionTime,ArrivalTime` (e.g., `10,0`).
5. **Run the executable:**
```bash
./scheduler

```



## 📊 Output

Upon running, the program will:

1. Read and parse `input.txt`.
2. Test multiple Time Quantum (`q`) values.
3. Print the Average Waiting Time (AWT) for each `q`.
4. Identify and report the **Best Quantum Value**.
5. Output a detailed trace log of the execution using the optimal `q`.
