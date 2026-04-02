#include "kernel/types.h"
#include "user/user.h"
#include "kernel/ptree.h"

#define max 5

void printTree(struct ptreeinfo *, int, int, int);
void inKhoangTrang(int);

int main (int argc, char *argv[]) {
    
    if (argc > 1) {
        fprintf(2, "Usage: pstree\n");
        exit(1);
    }

    struct ptreeinfo p_tree_info[max];
    int count = ptree(p_tree_info, max);

    if (count < 0) {
        printf("Error: system call failed (invalid buffer or max).\n");
        exit(1);
    }

    printTree(p_tree_info, count, 0, 0);
    exit(0);
}

void printTree(struct ptreeinfo * p_tree_info, int count, int parent_id, int depth) {
    for (int i = 0; i < count; i++) {
        if (p_tree_info[i].ppid == parent_id) {
            inKhoangTrang(depth * 2);
            printf("%d %s state=%d mem=%ld\n", p_tree_info[i].pid ,p_tree_info[i].name, p_tree_info[i].state, p_tree_info[i].memsize);
            printTree(p_tree_info, count, p_tree_info[i].pid, depth + 1);
        }
    }
}

void inKhoangTrang(int num) {
    for (int i = 0; i < num; i++) {
        printf(" ");
    }
}