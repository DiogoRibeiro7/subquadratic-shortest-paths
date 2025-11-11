#ifndef SSSP_BREAKTHROUGH_H
#define SSSP_BREAKTHROUGH_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <limits.h>
#include <stdbool.h>
#include <float.h>

/* Core constants and types */
#define INFINITY_DIST DBL_MAX
#define MAX_VERTICES 100000
#define MAX_EDGES 1000000

typedef struct Edge {
    int target;
    double weight;
    struct Edge* next;
} Edge;

typedef struct {
    int n, m;                    // Number of vertices and edges
    Edge** adj;                  // Adjacency list representation
    double* dist;                // Distance estimates db[v]
    int* pred;                   // Predecessor array
    bool* complete;              // Completeness status
    int source;                  // Source vertex
} Graph;

/* Data structure for Lemma 3.3 - Block-based linked list */
typedef struct Block {
    int* keys;
    double* values;
    int size;
    int capacity;
    double upper_bound;
    struct Block* next;
} Block;

typedef struct {
    Block* D0_head;              // Blocks from batch prepends
    Block* D1_head;              // Blocks from regular inserts
    int M;                       // Block size parameter
    double B;                    // Upper bound on values
    int total_elements;
} DataStructure;

/* Priority queue for base case */
typedef struct {
    int* vertices;
    double* distances;
    int* positions;              // Position of vertex in heap
    int size;
    int capacity;
} MinHeap;

/* Function declarations */

/* Graph operations */
Graph* create_graph(int n);
void add_edge(Graph* g, int u, int v, double weight);
void free_graph(Graph* g);
void relax_edge(Graph* g, int u, int v, double weight);

/* Data structure operations (Lemma 3.3) */
DataStructure* ds_create(int M, double B);
void ds_insert(DataStructure* ds, int key, double value);
void ds_batch_prepend(DataStructure* ds, int* keys, double* values, int count);
int ds_pull(DataStructure* ds, int* result_keys, double* result_values, 
           int max_count, double* separator);
bool ds_is_empty(DataStructure* ds);
void ds_free(DataStructure* ds);

/* Min heap operations */
MinHeap* heap_create(int capacity);
void heap_insert(MinHeap* heap, int vertex, double distance);
int heap_extract_min(MinHeap* heap);
void heap_decrease_key(MinHeap* heap, int vertex, double new_distance);
void heap_free(MinHeap* heap);

/* Core algorithm functions */
typedef struct {
    double boundary;
    int* vertices;
    int count;
} BMSSPResult;

/* FindPivots algorithm (Algorithm 1) */
typedef struct {
    int* pivots;
    int pivot_count;
    int* W;
    int W_count;
} PivotsResult;

PivotsResult* find_pivots(Graph* g, double B, int* S, int S_size, int k);
void free_pivots_result(PivotsResult* result);

/* Base case algorithm (Algorithm 2) */
BMSSPResult* base_case(Graph* g, double B, int source, int k);

/* Main BMSSP algorithm (Algorithm 3) */
BMSSPResult* bmssp(Graph* g, int level, double B, int* S, int S_size, 
                   int k, int t);

/* Main single-source shortest path algorithm */
void sssp_breakthrough(Graph* g, int source);

/* Control verbosity of the algorithm output */
void sssp_set_verbose(bool verbose);

/* Utility functions */
int floor_log_power(double x, double base);
void print_distances(Graph* g);
bool verify_solution(Graph* g);

/* Free result structures */
void free_bmssp_result(BMSSPResult* result);

#endif /* SSSP_BREAKTHROUGH_H */
