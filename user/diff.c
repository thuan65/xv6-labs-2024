#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "user/user.h"
#include <stdbool.h>

/*
  demo_line: Minh hoạ đọc dữ liệu theo DÒNG (line-based I/O).
  Dùng để làm các bài cần xử lý theo dòng: diff, rgrep, xargs (đọc input từng dòng), ...

  Vì sao cần đọc theo dòng?
  - read(fd, buf, n) có thể trả về đoạn dữ liệu cắt ngang dòng.
  - Với bài cần "line number" hoặc so sánh theo dòng, ta cần gom đến '\n'.

  Cú pháp:
    demo_line          (đọc stdin)
    demo_line <file>   (đọc file)

  Cách làm đơn giản (phù hợp Lab01): read từng ký tự 1 byte cho đến '\n' hoặc EOF.
*/

static int
readline(int fd, char *buf, int max)
{
  // Trả về:
  //  - số byte đọc được (>0) nếu đọc được ít nhất 1 ký tự
  //  - 0 nếu EOF ngay từ đầu
  //  - -1 nếu lỗi read

  int i = 0;
  char c;

  while (i + 1 < max) {          // chừa 1 byte cho '\0'
    int n = read(fd, &c, 1);
    if (n == 0) {                // EOF
      break;
    }
    if (n < 0) {                 // lỗi
      return -1;
    }
    buf[i++] = c;
    if (c == '\n') {             // kết thúc dòng
      break;
    }
  }

  buf[i] = '\0';
  return i;
}

int
main(int argc, char *argv[])
{
  int fd1 = 0; // mặc định stdin
  int fd2 = 0;

//   if (argc == 2) {
//     fd = open(argv[1], O_RDONLY);
//     if (fd < 0) {
//       fprintf(2, "demo_line: cannot open %s\n", argv[1]);
//       exit(1);
//     }
//   } else if (argc > 2) {
//     fprintf(2, "usage: demo_line [file]\n");
//     exit(1);
//   }

  fd1 = open(argv[1], O_RDONLY);
  fd2 = open(argv[2], O_RDONLY);

  if (argc == 4) {
    char mystrc[] = "-q";
    if (strcmp(argv[1], mystrc) == 0) {
        printf("...s");
    }
    else {
        printf("...");
        exit(1);
    }
  } 
  
  char buffer_line_file_1[256];
  char buffer_line_file_2[256];
  bool is_EOF_1 = false;
  bool is_EOF_2 = false;

  int lineNo = 1;

  while (1) {

    if (is_EOF_1 == false) {
      int n1 = readline(fd1, buffer_line_file_1, sizeof(buffer_line_file_1));
      if (n1 < 0) {
        fprintf(2, "demo_line: read error\n");
        if (fd1 != 0) close(fd1);
        exit(1);
      }
      else if (n1 == 0) {//EOF File1 
        is_EOF_1 = true;
      }
    }

    if (is_EOF_2 == false) {
      int n2 = readline(fd2, buffer_line_file_2, sizeof(buffer_line_file_2));
      if (n2 < 0) {
        fprintf(2, "demo_line: read error\n");
        if (fd2 != 0) close(fd2);
        exit(1);
      }
      else if (n2 == 0) {// EOF file 2
        is_EOF_2 = true;
      }
    }

    if (is_EOF_1 == true && is_EOF_2 == true) {
      break; //Both File end
    }

    if (is_EOF_1 == true) {
      printf("f1: %d: < EOF", lineNo);
      printf("f2: %d: < %s", lineNo, buffer_line_file_2);
      lineNo++;
      continue;
    }

    if (is_EOF_2 == true) {
      printf("f1: %d: < %s", lineNo, buffer_line_file_1);
      printf("f2: %d: < EOF", lineNo);
      lineNo++;
      continue;
    }

    if (strcmp(buffer_line_file_1, buffer_line_file_2) == 0) {
      lineNo++;
      continue;
    }
    else {
      printf("f1: %d: < %s", lineNo, buffer_line_file_1);
      printf("f2: %d: < %s", lineNo, buffer_line_file_2);
    }

    lineNo++;
  }

  if (fd1 != 0) close(fd1);
  if (fd2 != 0) close(fd2);

  exit(0);
}
