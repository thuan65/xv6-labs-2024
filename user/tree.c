#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"
#include "user/user.h"

void print_prefix(int level, int last_at_depth[]) {
    for (int i = 1; i < level; i++) {
        printf(last_at_depth[i] ? "    " : "│   ");
    }
    if (level > 0) {
        printf(last_at_depth[level] ? "└── " : "├── ");
    }
}

void build_chill_path(char *buffer, char *base, char *name) {
    strcpy(buffer, base);
    int len = strlen(buffer);
    buffer[len] = '/';
    strcpy(buffer + len + 1, name);
}

int validate_entry(char *path, struct dirent *de, int only_dir) {
    if (de->inum == 0 || strcmp(de->name, ".") == 0 || strcmp(de->name, "..") == 0) {
        return 0;
    }

    char child_path[512];
    struct stat child_st;

    build_chill_path(child_path, path, de->name);

    if (stat(child_path, &child_st) < 0) {
        return 0;
    }

    if (only_dir && child_st.type != T_DIR) {
        return 0;
    }

    return 1;
}

void recursive_tree(char *path, int level, int max_depth, int only_dir, int last_at_depth[]) {
    if (level > max_depth) {
        return;
    }

    int fd;
    struct dirent de;
    struct stat st;

    if (stat(path, &st) < 0) {
        fprintf(2, "tree: cannot stat %s\n", path);
        return;
    } 

    if (st.type != T_DIR) {
        return;
    }

    // First pass: count printable children
    if ((fd = open(path, O_RDONLY)) < 0) {
        fprintf(2, "tree: cannot open %s\n", path);
        return;
    }

    int valid_entries = 0;
    while (read(fd, &de, sizeof(de)) == sizeof(de)) {
        if (validate_entry(path, &de, only_dir)) {
            ++valid_entries;
        }
    }
    close(fd);

    // Second pass: print and recurse
    if ((fd = open(path, O_RDONLY)) < 0) {
        fprintf(2, "tree: cannot open %s\n", path);
        return;
    }

    int seen = 0;
    while (read(fd, &de, sizeof(de)) == sizeof(de)) {
        if (!validate_entry(path, &de, only_dir)) {
            continue;
        }

        ++seen;
        last_at_depth[level] = (seen == valid_entries);

        char child_path[512];
        struct stat child_st;

        build_chill_path(child_path, path, de.name);

        if (stat(child_path, &child_st) < 0) {
            fprintf(2, "tree: cannot stat %s\n", child_path);
            continue;
        }

        print_prefix(level, last_at_depth);
        printf("%s\n", de.name);

        if (child_st.type == T_DIR) {
            recursive_tree(child_path, level + 1, max_depth, only_dir, last_at_depth);
        }
    }

    close(fd);
}

int main(int argc, char *argv[]) {
    char *path = ".";
    int level = 1;
    int max_depth = 100;
    int only_dir = 0;
    int last_at_depth[100];
    struct stat st;

    for (int i = 0; i < (sizeof(last_at_depth) / sizeof(last_at_depth[0])); i++) {
        last_at_depth[i] = 1;
    }

    if (argc > 5) {
        fprintf(2, "tree: too many arguments\n");
        fprintf(2, "usage: tree [path] [-L depth] [-d]\n");
        exit(1);
    }

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-d") == 0) {
            only_dir = 1;
        } else if (strcmp(argv[i], "-L") == 0) {
            if (i + 1 >= argc) {
                fprintf(2, "tree: option -L requires depth argument\n");
                fprintf(2, "usage: tree [path] [-L depth] [-d]\n");
                exit(1);
            }
            max_depth = atoi(argv[++i]);
        } else if (argv[i][0] != '-') {
            path = argv[i];
        } else {
          fprintf(2, "tree: unrecognized option %s\n", argv[i]);
          fprintf(2, "usage: tree [path] [-L depth] [-d]\n");
          exit(1);
        }
    }

    if (stat(path, &st) < 0) {
        fprintf(2, "tree: cannot stat %s\n", path);
        exit(1);
    }

    if (st.type != T_DIR) {
        fprintf(2, "tree: not a directory: %s\n", path);
        fprintf(2, "usage: tree [path] [-L depth] [-d]\n");
        exit(1);
    }

    printf("%s\n", path);
    recursive_tree(path, level, max_depth, only_dir, last_at_depth);
    exit(0);
}