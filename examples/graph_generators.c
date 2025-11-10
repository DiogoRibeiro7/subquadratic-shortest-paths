/**
 * @file graph_generators.c
 * @brief Comprehensive graph generation tool for testing SSSP algorithms
 *
 * This program generates various types of graphs useful for testing:
 * - Random sparse graphs (Erdős-Rényi)
 * - Grid graphs (2D and 3D lattices)
 * - Scale-free networks (Barabási-Albert model)
 * - Transportation network simulations (road networks, airline routes)
 * - Worst-case graphs for Dijkstra's algorithm
 * - Complete graphs, trees, DAGs
 *
 * Export formats:
 * - Edge list (simple text format)
 * - Adjacency matrix (dense format)
 * - DIMACS (graph challenge format)
 * - DOT (GraphViz visualization)
 * - JSON (for web applications)
 *
 * Usage:
 *   ./graph_generators [options]
 *   ./graph_generators --help
 *   ./graph_generators --type sparse --vertices 1000 --output graph.txt
 */

#include "sssp_breakthrough.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

/*============================================================================
 * Additional Graph Generators
 *============================================================================*/

/**
 * @brief Generate scale-free network using Barabási-Albert model
 *
 * Creates a network where degree distribution follows power law.
 * New vertices preferentially attach to high-degree vertices.
 *
 * @param n Total number of vertices
 * @param m0 Number of initial vertices
 * @param m Number of edges per new vertex
 * @param min_weight Minimum edge weight
 * @param max_weight Maximum edge weight
 * @return Generated graph or NULL on error
 */
Graph* graph_generate_scale_free(int n, int m0, int m,
                                 double min_weight, double max_weight) {
    if (n <= m0 || m0 <= 0 || m <= 0 || m > m0) {
        fprintf(stderr, "Invalid parameters for scale-free graph\n");
        return NULL;
    }

    Graph* graph = create_graph(n);
    if (!graph) {
        return NULL;
    }

    // Start with m0 connected vertices (complete graph)
    for (int i = 0; i < m0; i++) {
        for (int j = i + 1; j < m0; j++) {
            double weight = random_double(min_weight, max_weight);
            add_edge(graph, i, j, weight);
            add_edge(graph, j, i, weight);
        }
    }

    // Track degree of each vertex for preferential attachment
    int* degree = (int*)calloc(n, sizeof(int));
    if (!degree) {
        free_graph(graph);
        return NULL;
    }

    // Initialize degrees for initial vertices
    for (int i = 0; i < m0; i++) {
        degree[i] = m0 - 1;
    }

    int total_degree = m0 * (m0 - 1);

    // Add remaining vertices one by one
    for (int new_vertex = m0; new_vertex < n; new_vertex++) {
        // Select m vertices to connect to (preferential attachment)
        bool* connected = (bool*)calloc(new_vertex, sizeof(bool));
        if (!connected) {
            free(degree);
            free_graph(graph);
            return NULL;
        }

        int connections_made = 0;

        while (connections_made < m && connections_made < new_vertex) {
            // Choose vertex with probability proportional to degree
            int cumulative = 0;
            int target_sum = random_int(0, total_degree - 1);

            int chosen = -1;
            for (int v = 0; v < new_vertex; v++) {
                cumulative += degree[v];
                if (cumulative > target_sum && !connected[v]) {
                    chosen = v;
                    break;
                }
            }

            // If not found (due to already connected), pick random unconnected
            if (chosen == -1) {
                for (int v = 0; v < new_vertex; v++) {
                    if (!connected[v]) {
                        chosen = v;
                        break;
                    }
                }
            }

            if (chosen != -1) {
                double weight = random_double(min_weight, max_weight);
                add_edge(graph, new_vertex, chosen, weight);
                add_edge(graph, chosen, new_vertex, weight);

                connected[chosen] = true;
                degree[chosen]++;
                degree[new_vertex]++;
                total_degree += 2;
                connections_made++;
            } else {
                break;
            }
        }

        free(connected);
    }

    free(degree);
    return graph;
}

/**
 * @brief Generate transportation network (road network simulation)
 *
 * Creates a realistic road network with:
 * - Grid-like structure with shortcuts
 * - Variable road speeds/weights
 * - Highway connections between distant nodes
 *
 * @param n Number of intersections
 * @param highway_prob Probability of highway connection
 * @return Generated graph or NULL on error
 */
Graph* graph_generate_road_network(int n, double highway_prob) {
    // Create base grid
    int side = (int)sqrt(n);
    Graph* graph = graph_generate_grid(side, side, 1.0, 10.0, false);

    if (!graph) {
        return NULL;
    }

    // Add diagonal shortcuts (local roads)
    for (int i = 0; i < side - 1; i++) {
        for (int j = 0; j < side - 1; j++) {
            int u = i * side + j;
            int v = (i + 1) * side + (j + 1);

            if (random_double(0.0, 1.0) < 0.3) {  // 30% chance of diagonal
                double weight = random_double(8.0, 15.0);  // Longer distances
                add_edge(graph, u, v, weight);
                add_edge(graph, v, u, weight);
            }
        }
    }

    // Add long-distance highways
    int num_highways = (int)(n * highway_prob);
    for (int h = 0; h < num_highways; h++) {
        int u = random_int(0, graph->n - 1);
        int v = random_int(0, graph->n - 1);

        if (u != v) {
            // Highways are fast (low weight) despite long distance
            double weight = random_double(0.5, 2.0);
            add_edge(graph, u, v, weight);
            add_edge(graph, v, u, weight);
        }
    }

    return graph;
}

/**
 * @brief Generate airline network (hub-and-spoke model)
 *
 * Creates a network with:
 * - Few hub airports (high degree)
 * - Many spoke airports (low degree)
 * - Direct flights mainly through hubs
 *
 * @param n Number of airports
 * @param num_hubs Number of hub airports
 * @return Generated graph or NULL on error
 */
Graph* graph_generate_airline_network(int n, int num_hubs) {
    if (num_hubs >= n || num_hubs <= 0) {
        fprintf(stderr, "Invalid number of hubs\n");
        return NULL;
    }

    Graph* graph = create_graph(n);
    if (!graph) {
        return NULL;
    }

    // First num_hubs vertices are hubs
    // Connect all hubs to each other
    for (int i = 0; i < num_hubs; i++) {
        for (int j = i + 1; j < num_hubs; j++) {
            // Short flights between hubs (frequent routes)
            double weight = random_double(1.0, 3.0);
            add_edge(graph, i, j, weight);
            add_edge(graph, j, i, weight);
        }
    }

    // Connect spoke airports to random hubs
    for (int spoke = num_hubs; spoke < n; spoke++) {
        // Each spoke connects to 1-3 hubs
        int num_connections = random_int(1, 3);

        for (int c = 0; c < num_connections; c++) {
            int hub = random_int(0, num_hubs - 1);

            // Spoke-to-hub flights (variable distance)
            double weight = random_double(2.0, 10.0);
            add_edge(graph, spoke, hub, weight);
            add_edge(graph, hub, spoke, weight);
        }

        // Occasionally add direct spoke-to-spoke connection
        if (random_double(0.0, 1.0) < 0.1 && spoke > num_hubs) {
            int other_spoke = random_int(num_hubs, spoke - 1);
            double weight = random_double(5.0, 15.0);
            add_edge(graph, spoke, other_spoke, weight);
            add_edge(graph, other_spoke, spoke, weight);
        }
    }

    return graph;
}

/**
 * @brief Generate worst-case graph for Dijkstra's algorithm
 *
 * Creates a graph where Dijkstra repeatedly updates priorities,
 * maximizing the number of decrease-key operations.
 *
 * Strategy: Create a chain where each step has a slight decrease,
 * forcing Dijkstra to update all vertices multiple times.
 *
 * @param n Number of vertices
 * @return Generated graph or NULL on error
 */
Graph* graph_generate_dijkstra_worst_case(int n) {
    if (n < 3) {
        fprintf(stderr, "Need at least 3 vertices for worst-case graph\n");
        return NULL;
    }

    Graph* graph = create_graph(n);
    if (!graph) {
        return NULL;
    }

    // Create a pattern that forces many priority updates
    // Strategy: Each vertex has multiple incoming edges with
    // carefully chosen weights to trigger updates

    // Main path from source to target
    for (int i = 0; i < n - 1; i++) {
        add_edge(graph, i, i + 1, (n - i) * 1.0);
    }

    // Add edges that will cause Dijkstra to update estimates
    // Create cross edges that provide shorter paths discovered late
    for (int i = 0; i < n - 2; i++) {
        for (int j = i + 2; j < n; j++) {
            // Add edge if it creates a situation where the path
            // through this edge is discovered after initial estimate
            if ((i + j) % 3 == 0) {  // Selective edges
                double weight = (j - i) * 0.9;  // Slightly better than sum
                add_edge(graph, i, j, weight);
            }
        }
    }

    // Add a very short edge at the end to force reconsideration
    if (n >= 4) {
        add_edge(graph, n - 3, n - 1, 0.5);
    }

    return graph;
}

/**
 * @brief Generate graph with negative edge weights (but no negative cycles)
 *
 * Useful for testing algorithms that handle negative weights.
 * Dijkstra fails on these, but Bellman-Ford and the breakthrough algorithm work.
 *
 * @param n Number of vertices
 * @param negative_prob Probability of an edge being negative
 * @return Generated graph or NULL on error
 */
Graph* graph_generate_negative_weights(int n, double negative_prob) {
    Graph* graph = create_graph(n);
    if (!graph) {
        return NULL;
    }

    // Create a DAG to avoid negative cycles
    int m = n * 3;  // Moderate number of edges

    for (int e = 0; e < m; e++) {
        int u = random_int(0, n - 2);
        int v = random_int(u + 1, n - 1);  // Ensure u < v (DAG property)

        double weight;
        if (random_double(0.0, 1.0) < negative_prob) {
            weight = random_double(-10.0, -1.0);  // Negative
        } else {
            weight = random_double(1.0, 10.0);    // Positive
        }

        add_edge(graph, u, v, weight);
    }

    return graph;
}

/**
 * @brief Generate small-world network (Watts-Strogatz model)
 *
 * Creates a network with high clustering and short average path length.
 * Models social networks and biological networks.
 *
 * @param n Number of vertices
 * @param k Each vertex connects to k nearest neighbors
 * @param beta Rewiring probability
 * @return Generated graph or NULL on error
 */
Graph* graph_generate_small_world(int n, int k, double beta) {
    if (k >= n || k < 2 || k % 2 != 0) {
        fprintf(stderr, "k must be even and less than n\n");
        return NULL;
    }

    Graph* graph = create_graph(n);
    if (!graph) {
        return NULL;
    }

    // Create ring lattice: each vertex connected to k/2 neighbors on each side
    for (int i = 0; i < n; i++) {
        for (int j = 1; j <= k / 2; j++) {
            int neighbor = (i + j) % n;
            double weight = random_double(1.0, 10.0);
            add_edge(graph, i, neighbor, weight);
            add_edge(graph, neighbor, i, weight);
        }
    }

    // Rewire edges with probability beta
    for (int i = 0; i < n; i++) {
        for (int j = 1; j <= k / 2; j++) {
            if (random_double(0.0, 1.0) < beta) {
                int old_neighbor = (i + j) % n;
                int new_neighbor = random_int(0, n - 1);

                // Avoid self-loops and duplicate edges
                while (new_neighbor == i || new_neighbor == old_neighbor) {
                    new_neighbor = random_int(0, n - 1);
                }

                double weight = random_double(1.0, 10.0);
                add_edge(graph, i, new_neighbor, weight);
            }
        }
    }

    return graph;
}

/*============================================================================
 * Export Functions
 *============================================================================*/

/**
 * @brief Export graph as adjacency matrix
 *
 * Format: n x n matrix where M[i][j] is the weight of edge i->j
 * (infinity if no edge exists)
 *
 * @param graph Graph to export
 * @param filename Output filename
 * @return true on success, false on error
 */
bool export_adjacency_matrix(const Graph* graph, const char* filename) {
    if (!graph || !filename) {
        return false;
    }

    FILE* fp = fopen(filename, "w");
    if (!fp) {
        fprintf(stderr, "Error: Cannot open %s for writing\n", filename);
        return false;
    }

    // Write dimensions
    fprintf(fp, "%d\n", graph->n);

    // Create matrix initialized to infinity
    for (int i = 0; i < graph->n; i++) {
        for (int j = 0; j < graph->n; j++) {
            double weight = INFINITY_DIST;

            if (i == j) {
                weight = 0.0;  // Diagonal is zero
            } else {
                // Search for edge i -> j
                Edge* edge = graph->adj[i];
                while (edge) {
                    if (edge->target == j) {
                        weight = edge->weight;
                        break;
                    }
                    edge = edge->next;
                }
            }

            if (weight == INFINITY_DIST) {
                fprintf(fp, "INF");
            } else {
                fprintf(fp, "%.6f", weight);
            }

            if (j < graph->n - 1) {
                fprintf(fp, " ");
            }
        }
        fprintf(fp, "\n");
    }

    fclose(fp);
    return true;
}

/**
 * @brief Export graph in JSON format
 *
 * Useful for web applications and JavaScript visualization libraries.
 *
 * @param graph Graph to export
 * @param filename Output filename
 * @return true on success, false on error
 */
bool export_json(const Graph* graph, const char* filename) {
    if (!graph || !filename) {
        return false;
    }

    FILE* fp = fopen(filename, "w");
    if (!fp) {
        fprintf(stderr, "Error: Cannot open %s for writing\n", filename);
        return false;
    }

    fprintf(fp, "{\n");
    fprintf(fp, "  \"vertices\": %d,\n", graph->n);
    fprintf(fp, "  \"edges\": %d,\n", graph->m);
    fprintf(fp, "  \"nodes\": [\n");

    // Write vertices
    for (int i = 0; i < graph->n; i++) {
        fprintf(fp, "    {\"id\": %d}", i);
        if (i < graph->n - 1) {
            fprintf(fp, ",");
        }
        fprintf(fp, "\n");
    }

    fprintf(fp, "  ],\n");
    fprintf(fp, "  \"links\": [\n");

    // Write edges
    int edge_count = 0;
    for (int u = 0; u < graph->n; u++) {
        Edge* edge = graph->adj[u];
        while (edge) {
            fprintf(fp, "    {\"source\": %d, \"target\": %d, \"weight\": %.6f}",
                    u, edge->target, edge->weight);

            edge_count++;
            if (edge_count < graph->m) {
                fprintf(fp, ",");
            }
            fprintf(fp, "\n");

            edge = edge->next;
        }
    }

    fprintf(fp, "  ]\n");
    fprintf(fp, "}\n");

    fclose(fp);
    return true;
}

/**
 * @brief Export graph statistics to file
 *
 * @param graph Graph to analyze
 * @param filename Output filename
 * @return true on success, false on error
 */
bool export_statistics(const Graph* graph, const char* filename) {
    if (!graph || !filename) {
        return false;
    }

    FILE* fp = fopen(filename, "w");
    if (!fp) {
        fprintf(stderr, "Error: Cannot open %s for writing\n", filename);
        return false;
    }

    GraphProperties props = graph_get_properties(graph);

    fprintf(fp, "Graph Statistics\n");
    fprintf(fp, "================\n\n");
    fprintf(fp, "Basic Properties:\n");
    fprintf(fp, "  Vertices (n):        %d\n", props.num_vertices);
    fprintf(fp, "  Edges (m):           %d\n", props.num_edges);
    fprintf(fp, "  Average degree:      %.2f\n", props.avg_degree);
    fprintf(fp, "  Maximum degree:      %d\n", props.max_degree);
    fprintf(fp, "\n");

    fprintf(fp, "Edge Weights:\n");
    fprintf(fp, "  Minimum weight:      %.6f\n", props.min_weight);
    fprintf(fp, "  Maximum weight:      %.6f\n", props.max_weight);
    fprintf(fp, "  Has negative edges:  %s\n", props.has_negative_weights ? "Yes" : "No");
    fprintf(fp, "\n");

    fprintf(fp, "Graph Structure:\n");
    fprintf(fp, "  Is connected:        %s\n", props.is_connected ? "Yes" : "No");
    fprintf(fp, "  Is DAG:              %s\n", props.is_dag ? "Yes" : "No");
    fprintf(fp, "  Has negative cycle:  %s\n", props.has_negative_cycle ? "Yes" : "No");
    fprintf(fp, "\n");

    // Density
    double max_edges = (double)props.num_vertices * (props.num_vertices - 1);
    double density = max_edges > 0 ? props.num_edges / max_edges : 0.0;
    fprintf(fp, "Graph Density:         %.6f\n", density);

    fclose(fp);
    return true;
}

/*============================================================================
 * Main Menu and Interface
 *============================================================================*/

typedef struct {
    const char* name;
    const char* description;
    const char* type_code;
} GraphTypeInfo;

static const GraphTypeInfo graph_types[] = {
    {"Random Sparse", "Random graph with low density (avg degree ~3)", "sparse"},
    {"Random Medium", "Random graph with medium density (avg degree ~10)", "medium"},
    {"Random Dense", "Random graph with high density (avg degree ~50)", "dense"},
    {"Grid 2D", "Regular 2D grid lattice", "grid"},
    {"Grid 3D", "Regular 3D grid lattice (cube)", "grid3d"},
    {"Complete", "All vertices connected to all others", "complete"},
    {"Tree", "Random spanning tree", "tree"},
    {"DAG", "Directed acyclic graph", "dag"},
    {"Path", "Simple path from 0 to n-1", "path"},
    {"Scale-Free", "Barabási-Albert preferential attachment", "scale_free"},
    {"Road Network", "Transportation network (grid + highways)", "road"},
    {"Airline Network", "Hub-and-spoke network", "airline"},
    {"Small World", "Watts-Strogatz small-world network", "small_world"},
    {"Dijkstra Worst", "Worst-case for Dijkstra's algorithm", "dijkstra_worst"},
    {"Negative Weights", "Graph with negative edges (no cycles)", "negative"},
};

static const int num_graph_types = sizeof(graph_types) / sizeof(graph_types[0]);

void print_menu(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║              Graph Generator - Available Types               ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");

    for (int i = 0; i < num_graph_types; i++) {
        printf("%2d. %-18s - %s\n", i + 1,
               graph_types[i].name,
               graph_types[i].description);
    }
    printf("\n");
}

void print_usage(const char* program) {
    printf("Usage: %s [options]\n\n", program);
    printf("Options:\n");
    printf("  -t, --type TYPE       Graph type (see menu for options)\n");
    printf("  -n, --vertices N      Number of vertices (default: 100)\n");
    printf("  -o, --output FILE     Output filename (default: graph.txt)\n");
    printf("  -f, --format FORMAT   Output format: edge_list, matrix, dimacs, dot, json\n");
    printf("                        (default: edge_list)\n");
    printf("  -s, --stats FILE      Export statistics to file\n");
    printf("  --seed SEED           Random seed (default: time-based)\n");
    printf("  --visualize           Also generate DOT file for visualization\n");
    printf("  -h, --help            Show this help message\n");
    printf("  --menu                Show interactive menu\n");
    printf("\n");
    printf("Examples:\n");
    printf("  %s --type sparse --vertices 1000 --output sparse_1k.txt\n", program);
    printf("  %s --type scale_free -n 500 -f json -o network.json\n", program);
    printf("  %s --type road -n 100 --visualize\n", program);
    printf("  %s --menu\n", program);
    printf("\n");
}

Graph* generate_graph_by_type(const char* type, int n) {
    Graph* graph = NULL;

    if (strcmp(type, "sparse") == 0) {
        graph = graph_generate_sparse(n, 3, 1.0, 100.0);
    } else if (strcmp(type, "medium") == 0) {
        graph = graph_generate_sparse(n, 10, 1.0, 100.0);
    } else if (strcmp(type, "dense") == 0) {
        graph = graph_generate_sparse(n, 50, 1.0, 100.0);
    } else if (strcmp(type, "grid") == 0) {
        int side = (int)sqrt(n);
        graph = graph_generate_grid(side, side, 1.0, 100.0, false);
    } else if (strcmp(type, "grid3d") == 0) {
        int side = (int)cbrt(n);
        // Generate 3D grid by creating layers
        graph = create_graph(side * side * side);
        for (int z = 0; z < side; z++) {
            for (int y = 0; y < side; y++) {
                for (int x = 0; x < side; x++) {
                    int u = z * side * side + y * side + x;

                    // Connect to neighbors in 3D
                    if (x + 1 < side) {
                        int v = z * side * side + y * side + (x + 1);
                        double w = random_double(1.0, 10.0);
                        add_edge(graph, u, v, w);
                        add_edge(graph, v, u, w);
                    }
                    if (y + 1 < side) {
                        int v = z * side * side + (y + 1) * side + x;
                        double w = random_double(1.0, 10.0);
                        add_edge(graph, u, v, w);
                        add_edge(graph, v, u, w);
                    }
                    if (z + 1 < side) {
                        int v = (z + 1) * side * side + y * side + x;
                        double w = random_double(1.0, 10.0);
                        add_edge(graph, u, v, w);
                        add_edge(graph, v, u, w);
                    }
                }
            }
        }
    } else if (strcmp(type, "complete") == 0) {
        graph = graph_generate_complete(n, 1.0, 100.0);
    } else if (strcmp(type, "tree") == 0) {
        graph = graph_generate_tree(n, 1.0, 100.0);
    } else if (strcmp(type, "dag") == 0) {
        graph = graph_generate_dag(n, n * 3, 1.0, 100.0);
    } else if (strcmp(type, "path") == 0) {
        graph = graph_generate_path(n, 1.0, 100.0);
    } else if (strcmp(type, "scale_free") == 0) {
        int m0 = 3;  // Initial connected vertices
        int m = 2;   // Edges per new vertex
        graph = graph_generate_scale_free(n, m0, m, 1.0, 100.0);
    } else if (strcmp(type, "road") == 0) {
        graph = graph_generate_road_network(n, 0.05);  // 5% highways
    } else if (strcmp(type, "airline") == 0) {
        int hubs = (int)(n * 0.1);  // 10% are hubs
        if (hubs < 2) hubs = 2;
        graph = graph_generate_airline_network(n, hubs);
    } else if (strcmp(type, "small_world") == 0) {
        int k = 4;  // Connect to 4 nearest neighbors
        double beta = 0.1;  // 10% rewiring probability
        graph = graph_generate_small_world(n, k, beta);
    } else if (strcmp(type, "dijkstra_worst") == 0) {
        graph = graph_generate_dijkstra_worst_case(n);
    } else if (strcmp(type, "negative") == 0) {
        graph = graph_generate_negative_weights(n, 0.3);  // 30% negative edges
    } else {
        fprintf(stderr, "Unknown graph type: %s\n", type);
        return NULL;
    }

    return graph;
}

bool export_graph(const Graph* graph, const char* filename, const char* format) {
    if (strcmp(format, "edge_list") == 0) {
        return graph_write_edge_list(graph, filename);
    } else if (strcmp(format, "matrix") == 0) {
        return export_adjacency_matrix(graph, filename);
    } else if (strcmp(format, "dimacs") == 0) {
        return graph_write_dimacs(graph, filename, 0);
    } else if (strcmp(format, "dot") == 0) {
        return graph_write_dot(graph, filename, NULL);
    } else if (strcmp(format, "json") == 0) {
        return export_json(graph, filename);
    } else {
        fprintf(stderr, "Unknown format: %s\n", format);
        return false;
    }
}

int interactive_menu(void) {
    int choice;
    int n;
    char output_file[256];
    char format[64];
    bool generate_stats = false;
    bool visualize = false;

    print_menu();

    printf("Select graph type (1-%d): ", num_graph_types);
    if (scanf("%d", &choice) != 1 || choice < 1 || choice > num_graph_types) {
        fprintf(stderr, "Invalid selection\n");
        return 1;
    }

    printf("Number of vertices: ");
    if (scanf("%d", &n) != 1 || n <= 0) {
        fprintf(stderr, "Invalid number of vertices\n");
        return 1;
    }

    printf("Output filename: ");
    if (scanf("%255s", output_file) != 1) {
        fprintf(stderr, "Invalid filename\n");
        return 1;
    }

    printf("Format (edge_list/matrix/dimacs/dot/json) [edge_list]: ");
    if (scanf("%63s", format) != 1) {
        strcpy(format, "edge_list");
    }

    printf("Generate statistics? (y/n) [n]: ");
    char stats_choice;
    scanf(" %c", &stats_choice);
    generate_stats = (stats_choice == 'y' || stats_choice == 'Y');

    printf("Generate visualization? (y/n) [n]: ");
    char viz_choice;
    scanf(" %c", &viz_choice);
    visualize = (viz_choice == 'y' || viz_choice == 'Y');

    printf("\nGenerating %s with %d vertices...\n",
           graph_types[choice - 1].name, n);

    Graph* graph = generate_graph_by_type(graph_types[choice - 1].type_code, n);
    if (!graph) {
        fprintf(stderr, "Failed to generate graph\n");
        return 1;
    }

    printf("Generated graph: %d vertices, %d edges\n", graph->n, graph->m);

    // Export graph
    printf("Exporting to %s (%s format)...\n", output_file, format);
    if (!export_graph(graph, output_file, format)) {
        fprintf(stderr, "Failed to export graph\n");
        free_graph(graph);
        return 1;
    }
    printf("Successfully exported!\n");

    // Generate statistics
    if (generate_stats) {
        char stats_file[512];
        snprintf(stats_file, sizeof(stats_file), "%s.stats", output_file);
        printf("Generating statistics: %s\n", stats_file);
        export_statistics(graph, stats_file);
    }

    // Generate visualization
    if (visualize) {
        char dot_file[512];
        snprintf(dot_file, sizeof(dot_file), "%s.dot", output_file);
        printf("Generating DOT file: %s\n", dot_file);
        graph_write_dot(graph, dot_file, NULL);
        printf("Visualize with: dot -Tpng %s -o %s.png\n", dot_file, output_file);
    }

    free_graph(graph);
    return 0;
}

int main(int argc, char** argv) {
    // Default parameters
    const char* type = "sparse";
    int n = 100;
    const char* output = "graph.txt";
    const char* format = "edge_list";
    const char* stats_file = NULL;
    unsigned int seed = 0;
    bool visualize = false;
    bool menu_mode = false;

    // Parse command line arguments
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            print_usage(argv[0]);
            return 0;
        } else if (strcmp(argv[i], "--menu") == 0) {
            menu_mode = true;
        } else if ((strcmp(argv[i], "-t") == 0 || strcmp(argv[i], "--type") == 0) && i + 1 < argc) {
            type = argv[++i];
        } else if ((strcmp(argv[i], "-n") == 0 || strcmp(argv[i], "--vertices") == 0) && i + 1 < argc) {
            n = atoi(argv[++i]);
        } else if ((strcmp(argv[i], "-o") == 0 || strcmp(argv[i], "--output") == 0) && i + 1 < argc) {
            output = argv[++i];
        } else if ((strcmp(argv[i], "-f") == 0 || strcmp(argv[i], "--format") == 0) && i + 1 < argc) {
            format = argv[++i];
        } else if ((strcmp(argv[i], "-s") == 0 || strcmp(argv[i], "--stats") == 0) && i + 1 < argc) {
            stats_file = argv[++i];
        } else if (strcmp(argv[i], "--seed") == 0 && i + 1 < argc) {
            seed = (unsigned int)atoi(argv[++i]);
        } else if (strcmp(argv[i], "--visualize") == 0) {
            visualize = true;
        }
    }

    // Interactive menu mode
    if (menu_mode || argc == 1) {
        return interactive_menu();
    }

    // Set random seed
    utils_set_random_seed(seed);

    // Generate graph
    printf("Generating %s graph with %d vertices...\n", type, n);
    Graph* graph = generate_graph_by_type(type, n);

    if (!graph) {
        fprintf(stderr, "Failed to generate graph\n");
        return 1;
    }

    printf("Generated: %d vertices, %d edges\n", graph->n, graph->m);

    // Export graph
    printf("Exporting to %s (%s format)...\n", output, format);
    if (!export_graph(graph, output, format)) {
        fprintf(stderr, "Failed to export graph\n");
        free_graph(graph);
        return 1;
    }
    printf("Successfully exported to: %s\n", output);

    // Export statistics if requested
    if (stats_file) {
        printf("Generating statistics: %s\n", stats_file);
        export_statistics(graph, stats_file);
    }

    // Generate visualization if requested
    if (visualize) {
        char dot_file[512];
        snprintf(dot_file, sizeof(dot_file), "%s.dot", output);
        printf("Generating DOT file: %s\n", dot_file);
        graph_write_dot(graph, dot_file, NULL);
        printf("\nVisualize with:\n");
        printf("  dot -Tpng %s -o %s.png\n", dot_file, output);
        printf("  dot -Tsvg %s -o %s.svg\n", dot_file, output);
    }

    free_graph(graph);

    printf("\n");
    printf("Done! Graph generation complete.\n");

    return 0;
}
