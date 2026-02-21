// Arda Cenker Karagöz
// 150123012

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#define MAX_PROCESSES 100

// --- DATA STRUCTURES ---

// Structure representing a Process
typedef struct Process
{
    int id;
    int execution_time;
    int remaining_time;
    int arrival_time;
    double priority; // Dynamic priority value (lower is better)
} Process;

// Binomial Heap Node structure
typedef struct BHNode
{
    Process data;
    int degree; // Number of children
    struct BHNode *parent;
    struct BHNode *child;   // Pointer to the first child
    struct BHNode *sibling; // Pointer to the next sibling
} BHNode;

// Binomial Heap structure
typedef struct BinomialHeap
{
    BHNode *head; // Pointer to the first root in the heap
} BinomialHeap;

// --- HELPER FUNCTIONS ---

// Initializes a new empty Binomial Heap
BinomialHeap *heapInit()
{
    BinomialHeap *heap = (BinomialHeap *)malloc(sizeof(BinomialHeap));
    heap->head = NULL;
    return heap;
}

// Creates a new heap node for a given process
BHNode *nodeInit(Process p)
{
    BHNode *node = (BHNode *)malloc(sizeof(BHNode));
    node->data = p;
    node->degree = 0;
    node->parent = NULL;
    node->child = NULL;
    node->sibling = NULL;
    return node;
}

// Links two trees: Makes tree 'y' a child of tree 'z'
void heapLink(BHNode *y, BHNode *z)
{
    y->parent = z;
    y->sibling = z->child;
    z->child = y;
    z->degree++;
}

// Recursively frees memory (Optional cleanup)
void freeNode(BHNode *node)
{
    if (node == NULL)
        return;
    freeNode(node->child);
    freeNode(node->sibling);
    free(node);
}

void heapFree(BinomialHeap *heap)
{
    freeNode(heap->head);
    free(heap);
}

// --- CORE HEAP OPERATIONS ---

// Merges two binomial heaps into one sorted by degree
BHNode *heapMerge(BinomialHeap *h1, BinomialHeap *h2)
{
    if (h1->head == NULL)
        return h2->head;
    if (h2->head == NULL)
        return h1->head;

    BHNode *head;
    BHNode *h1It = h1->head;
    BHNode *h2It = h2->head;
    BHNode *tail;

    if (h1It->degree <= h2It->degree)
    {
        head = h1It;
        h1It = h1It->sibling;
    }
    else
    {
        head = h2It;
        h2It = h2It->sibling;
    }
    tail = head;

    while (h1It != NULL && h2It != NULL)
    {
        if (h1It->degree <= h2It->degree)
        {
            tail->sibling = h1It;
            h1It = h1It->sibling;
        }
        else
        {
            tail->sibling = h2It;
            h2It = h2It->sibling;
        }
        tail = tail->sibling;
    }

    if (h1It != NULL)
        tail->sibling = h1It;
    else
        tail->sibling = h2It;

    return head;
}

// Unites two binomial heaps, ensuring the heap property is maintained
void heapUnion(BinomialHeap *mainHeap, BinomialHeap *otherHeap)
{
    BHNode *new_head = heapMerge(mainHeap, otherHeap);

    if (new_head == NULL)
    {
        mainHeap->head = NULL;
        return;
    }

    BHNode *prev = NULL;
    BHNode *aux = new_head;
    BHNode *next = aux->sibling;

    while (next != NULL)
    {
        if ((aux->degree != next->degree) ||
            (next->sibling != NULL && next->sibling->degree == aux->degree))
        {
            prev = aux;
            aux = next;
        }
        else
        {
            if (aux->data.priority <= next->data.priority)
            {
                aux->sibling = next->sibling;
                heapLink(next, aux);
            }
            else
            {
                if (prev == NULL)
                    new_head = next;
                else
                    prev->sibling = next;
                heapLink(aux, next);
                aux = next;
            }
        }
        next = aux->sibling;
    }
    mainHeap->head = new_head;
}

// Inserts a new process into the heap
void heapInsert(BinomialHeap *heap, Process p)
{
    BinomialHeap *tempHeap = heapInit();
    tempHeap->head = nodeInit(p);
    heapUnion(heap, tempHeap);
    free(tempHeap);
}

// Finds the node with the minimum priority value
BHNode *heapMin(BinomialHeap *heap)
{
    if (heap->head == NULL)
        return NULL;
    BHNode *curr = heap->head;
    BHNode *minNode = curr;
    double minVal = curr->data.priority;
    while (curr != NULL)
    {
        if (curr->data.priority < minVal)
        {
            minVal = curr->data.priority;
            minNode = curr;
        }
        curr = curr->sibling;
    }
    return minNode;
}

// Extracts (removes and returns) the node with the minimum priority
BHNode *heapRemove(BinomialHeap *heap)
{
    if (heap->head == NULL)
        return NULL;

    BHNode *curr = heap->head;
    BHNode *prev = NULL;
    BHNode *minNode = curr;
    BHNode *minPrev = NULL;
    double minVal = curr->data.priority;

    while (curr != NULL)
    {
        if (curr->data.priority < minVal)
        {
            minVal = curr->data.priority;
            minNode = curr;
            minPrev = prev;
        }
        prev = curr;
        curr = curr->sibling;
    }

    if (minPrev == NULL)
    {
        heap->head = minNode->sibling;
    }
    else
    {
        minPrev->sibling = minNode->sibling;
    }

    BHNode *child = minNode->child;
    BHNode *newHead = NULL;
    while (child != NULL)
    {
        BHNode *next = child->sibling;
        child->sibling = newHead;
        child->parent = NULL;
        newHead = child;
        child = next;
    }

    BinomialHeap *childHeap = heapInit();
    childHeap->head = newHead;
    heapUnion(heap, childHeap);
    free(childHeap);

    return minNode;
}

// --- UTILITY FUNCTIONS ---

// Calculates the dynamic priority
double calculatePriority(Process p, int e_max)
{
    // First arrival instance (If not executed yet, priority = execution time)
    if (p.remaining_time == p.execution_time)
    {
        return (double)p.execution_time;
    }

    double ratio = (2.0 * p.remaining_time) / (3.0 * e_max);

    double exponent = pow(ratio, 3.0);

    return p.remaining_time * exp(exponent);
}

// Reads input file
int read_input_file(char *filename, Process *processes)
{
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        printf("Error: Could not open file %s\n", filename);
        return -1;
    }
    int count = 0;
    int exec_t, arr_t;
    while (fscanf(file, "%d,%d", &exec_t, &arr_t) == 2)
    {
        if (count >= MAX_PROCESSES)
            break;
        processes[count].id = count + 1;
        processes[count].execution_time = exec_t;
        processes[count].remaining_time = exec_t;
        processes[count].arrival_time = arr_t;
        processes[count].priority = 0;
        count++;
    }
    fclose(file);
    return count;
}

// Prints heap contents (recursive helper)
void printHeapContents(BHNode *node)
{
    while (node != NULL)
    {
        printf("P%d: %.3f ", node->data.id, node->data.priority);
        node = node->sibling;
    }
}

// --- SIMULATION ENGINE ---

double run_simulation(int q, Process original_processes[], int process_count, int e_max, int detailed_log)
{

    Process *process = (Process *)malloc(sizeof(Process) * process_count);
    for (int i = 0; i < process_count; i++)
        process[i] = original_processes[i];

    BinomialHeap *heap = heapInit();
    double current_time = 0.0;
    int completed_processes = 0;
    int next_process_idx = 0;
    double total_waiting_time = 0;

    if (detailed_log)
    {
        printf("\nDetailed Simulation Log for q=%d:\n", q);
        printf("Time\tCPU\tHeap Contents (Process: Priority)\n");
        printf("----\t---\t---------------------------------\n");
    }

    while (completed_processes < process_count)
    {
        // 1. ARRIVAL: Enqueue processes that have arrived by current_time
        while (next_process_idx < process_count &&
               (double)process[next_process_idx].arrival_time <= current_time)
        {
            process[next_process_idx].priority = calculatePriority(process[next_process_idx], e_max);
            heapInsert(heap, process[next_process_idx]);
            next_process_idx++;
        }

        // 2. IDLE CHECK: If there are no tasks, advance time
        if (heap->head == NULL)
        {
            if (detailed_log)
                printf("%.1f\tIDLE\t-\n", current_time);
            current_time += 1.0;
            continue;
        }

        // 3. SCHEDULE: Select the highest priority process
        BHNode *minNode = heapRemove(heap);
        Process p = minNode->data;
        free(minNode);

        // 4. EXECUTION: Determine run time
        int run_time = q;
        if (p.remaining_time < q)
        {
            run_time = p.remaining_time;
        }

        // --- LOGGING (START TIME ONLY - SINGLE LINE) ---
        if (detailed_log)
        {
            // Print Time and Process ID
            printf("%.1f\tP%d\t", current_time, p.id);

            // Print the priority of the running process (Initial value)
            printf("P%d: %.3f ", p.id, p.priority);

            // Print waiting processes next to it
            if (heap->head != NULL)
            {
                printHeapContents(heap->head);
            }

            printf("\n");
        }

        // Update time and remaining time by "jumping" forward
        current_time += (double)run_time;
        p.remaining_time -= run_time;

        // 5. NEW ARRIVALS: Check for processes that arrived during execution
        while (next_process_idx < process_count &&
               (double)process[next_process_idx].arrival_time <= current_time)
        {
            process[next_process_idx].priority = calculatePriority(process[next_process_idx], e_max);
            heapInsert(heap, process[next_process_idx]);
            next_process_idx++;
        }

        // --- CONTEXT SWITCH DELAY ---
        double delay = 0.0;
        if (heap->head != NULL)
        {
            delay = 0.1;
        }

        if (p.remaining_time == 0)
        {
            completed_processes++;
            double waiting_time = current_time - (double)p.arrival_time - (double)p.execution_time;
            total_waiting_time += waiting_time;

            current_time += delay;
        }
        else
        {
            current_time += delay;
            p.priority = calculatePriority(p, e_max); // Update priority
            heapInsert(heap, p);                      // Re-queue the process
        }
    }

    // --- END: Print EMPTY when finished ---
    if (detailed_log)
    {
        printf("%.1f\tEMPTY\n", current_time);
    }

    free(process);
    heapFree(heap);
    return total_waiting_time / process_count;
}

// --- MAIN ---

int main()
{
    Process processes[MAX_PROCESSES];

    printf("Reading process data from input.txt...\n");
    int process_count = read_input_file("input.txt", processes);

    if (process_count <= 0)
    {
        printf("No processes found or error reading file.\n");
        return 1;
    }
    printf("Successfully read %d processes.\n", process_count);

    int e_max = 0;
    for (int i = 0; i < process_count; i++)
    {
        if (processes[i].execution_time > e_max)
        {
            e_max = processes[i].execution_time;
        }
    }
    printf("Calculated e_max: %d\n", e_max);

    int best_q = -1;
    double min_awt = 1e9;

    // Iterate q from 1 to e_max + 2
    for (int q = 1; q <= e_max + 2; q++)
    {
        double awt = run_simulation(q, processes, process_count, e_max, 0);
        printf("q=%d -> AWT: %.4f\n", q, awt);

        if (awt < min_awt)
        {
            min_awt = awt;
            best_q = q;
        }
    }

    printf("\n--- Final Report ---\n");
    printf("Best Quantum Value (q): %d\n", best_q);
    printf("Minimum Average Waiting Time: %.4f\n", min_awt);

    // Final detailed run
    run_simulation(best_q, processes, process_count, e_max, 1);

    return 0;
}