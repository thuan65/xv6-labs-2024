#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"
#include "user/user.h"

// In thụt lề theo level
void print_prefix(int level) {
  for (int i = 0; i < level; i++) {
    printf("    ");
  }
  if (level > 0) {
    printf("|-- ");
  }
}

// Tạo đường dẫn đầy đủ: base/name (tức là tạo đường dẫn tuyệt đối cho mục con để đưa trở lại vào hàm duyệt cây thư mục)
void build_path(char *buf, char *base, char *name) {
  strcpy(buf, base);
  char *p = buf + strlen(buf);
  *p++ = '/';
  strcpy(p, name);
}

// Hàm đệ quy duyệt cây thư mục
void tree_recursive(char *path, int level, int maxDepth, int onlyDir) {

  if (level > maxDepth) {
    return;
  }

  int fd;
  struct dirent de;
  struct stat st;


  if ((fd = open(path, O_RDONLY)) < 0) {
    fprintf(2, "tree: cannot open %s\n", path);
    return;
  }


  if (fstat(fd, &st) < 0) {
    fprintf(2, "tree: cannot get stat %s\n", path);
    close(fd);
    return;
  }

  // return nếu là file vì trong vòng lặp while đã in ra rồi
  if (st.type != T_DIR) {
    close(fd);
    return;
  }

  // Đọc từng entry trong thư mục
  while (read(fd, &de, sizeof(de)) == sizeof(de)) {

    if (de.inum == 0) {
      continue;
    }

    // Bỏ qua "." và ".." vì nó gây loop vô hạn khi đệ quy
    if (strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0) {
      continue;
    }

    // Tạo đường dẫn đầy đủ cho entry con để lát đưa nó vào hàm đệ quy
    char fullpath[512];
    build_path(fullpath, path, de.name);


    if (fstat(fd, &st) < 0) {
      fprintf(2, "tree: cannot get stat %s\n", fullpath);
      continue;
    }


    if (onlyDir && st.type != T_DIR) {
      continue;
    }


    print_prefix(level);
    printf("%s\n", de.name);


    if (st.type == T_DIR) {
      tree_recursive(fullpath, level + 1, maxDepth, onlyDir);
    }
  }

  close(fd);
}

int main(int argc, char *argv[]) {
  char *path = ".";
  int maxDepth = 999;
  int onlyDir = 0;


  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "-d") == 0) {
      onlyDir = 1;
    } else if (strcmp(argv[i], "-L") == 0) {
      if (i + 1 < argc) {
        maxDepth = atoi(argv[i + 1]);
        i++;
      } else {
        fprintf(2, "tree: -L requires depth argument\n");
        exit(1);
      }
    } else if (argv[i][0] != '-') {
      path = argv[i];
    }
  }


  printf("%s\n", path);


  tree_recursive(path, 0, maxDepth - 1, onlyDir);

  exit(0);
}