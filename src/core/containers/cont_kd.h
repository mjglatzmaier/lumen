#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// Structure for a kd-tree node
typedef struct KdNode {
    float *point;   // k-dimensional point
    int axis;       // Splitting axis
    struct KdNode *left, *right;
} KdNode;

// Compare function for sorting along a specific axis
typedef struct {
    float **points;
    int axis;
} SortContext;

int compare_points(const void *a, const void *b, void *context) {
    SortContext *ctx = (SortContext *)context;
    float *p1 = *(float **)a;
    float *p2 = *(float **)b;
    return (p1[ctx->axis] < p2[ctx->axis]) ? -1 : (p1[ctx->axis] > p2[ctx->axis]);
}

// Function to build the kd-tree recursively
KdNode *build_kd_tree(float **points, int n, int k, int depth) {
    if (n <= 0) return NULL;
    
    int axis = depth % k;
    SortContext ctx = {points, axis};
    qsort_r(points, n, sizeof(float *), compare_points, &ctx);
    
    int median = n / 2;
    KdNode *node = (KdNode *)malloc(sizeof(KdNode));
    node->point = points[median];
    node->axis = axis;
    node->left = build_kd_tree(points, median, k, depth + 1);
    node->right = build_kd_tree(points + median + 1, n - median - 1, k, depth + 1);
    
    return node;
}

// Function to compute squared Euclidean distance
float squared_distance(float *a, float *b, int k) {
    float dist = 0.0f;
    for (int i = 0; i < k; i++)
        dist += (a[i] - b[i]) * (a[i] - b[i]);
    return dist;
}

// Nearest Neighbor Search
void nearest_neighbor(KdNode *root, float *target, int k, KdNode **best, float *best_dist) {
    if (!root) return;
    
    float dist = squared_distance(root->point, target, k);
    if (!(*best) || dist < *best_dist) {
        *best = root;
        *best_dist = dist;
    }
    
    int axis = root->axis;
    KdNode *next = (target[axis] < root->point[axis]) ? root->left : root->right;
    KdNode *other = (next == root->left) ? root->right : root->left;
    
    nearest_neighbor(next, target, k, best, best_dist);
    if (fabs(target[axis] - root->point[axis]) < sqrt(*best_dist))
        nearest_neighbor(other, target, k, best, best_dist);
}

// Helper function to find the nearest neighbor
KdNode *find_nearest(KdNode *root, float *target, int k) {
    KdNode *best = NULL;
    float best_dist = INFINITY;
    nearest_neighbor(root, target, k, &best, &best_dist);
    return best;
}

// Free kd-tree
void free_kd_tree(KdNode *root) {
    if (!root) return;
    free_kd_tree(root->left);
    free_kd_tree(root->right);
    free(root);
}

// // Example usage
// int main() {
//     int k = 3; // 3D points
//     int n = 5;
//     float points_data[5][3] = {{2.0, 3.0, 4.0}, {5.0, 4.0, 2.0}, {9.0, 6.0, 7.0}, {4.0, 7.0, 9.0}, {8.0, 1.0, 5.0}};
//     float *points[5];
//     for (int i = 0; i < n; i++)
//         points[i] = points_data[i];
    
//     KdNode *root = build_kd_tree(points, n, k, 0);
//     float target[3] = {5.0, 5.0, 5.0};
//     KdNode *nearest = find_nearest(root, target, k);
//     printf("Nearest point to (%f, %f, %f) is (%f, %f, %f)\n", target[0], target[1], target[2], nearest->point[0], nearest->point[1], nearest->point[2]);
    
//     free_kd_tree(root);
//     return 0;
// }
