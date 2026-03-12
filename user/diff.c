#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h" 
#include "kernel/fcntl.h" //For O_RDONLY
#include <stdbool.h> //For the bool true false

/*
 * diff compare the contents of two files line-by-line.
 *
 * - Standard read() calls may return data in arbitrary chunks.
 * - Provide feedback (like line numbers) and buffer characters
 * until a newline '\n' is encountered.
 *
 * Supported Modes:
 * - Default: Displays the specific line numbers and content differences.
 * - Quiet (-q): Only reports whether files differ without showing details.
 *
 * Usage:
 * diff <file1> <file2>      (Standard mode)
 * diff -q <file1> <file2>   (Quiet mode)
 *
 */

#define MAX_LINE 256

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

void diff(char* file1, char* file2, int quiet) {//quite == 0 false , quiet == 1 true
  int fd1, fd2;
  char buffer_line_file_1[MAX_LINE], buffer_line_file_2[MAX_LINE];
  bool is_EOF_1 = false;
  bool is_EOF_2 = false;
  int n1, n2;
  bool isDiff = false;
  int lineNo = 1;

  if ((fd1 = open(file1, O_RDONLY)) < 0) {
    fprintf(2, "diff: cannot open %s\n", file1);
    return;
  }
  if ((fd2 = open(file2, O_RDONLY)) < 0) {
    close(fd1); //close fd1 if error
    fprintf(2, "diff: cannot open %s\n", file2);
    return;
  }

  while (1) {
    //This block is for read line from two file 1
    if (is_EOF_1 == false) {
      n1 = readline(fd1, buffer_line_file_1, sizeof(buffer_line_file_1));
      if (n1 < 0) {
        fprintf(2, "demo_line: read error\n");
        if (fd1 != 0) close(fd1);
        exit(1);
      }
      else if (n1 == 0) {//EOF File1 
        is_EOF_1 = true;
      }
    }
       //This block is for read line from two file 2
    if (is_EOF_2 == false) {
      n2 = readline(fd2, buffer_line_file_2, sizeof(buffer_line_file_2));
      if (n2 < 0) {
        fprintf(2, "demo_line: read error\n");
        if (fd2 != 0) close(fd2);
        exit(1);
      }
      else if (n2 == 0) {// EOF file 2
        is_EOF_2 = true;
      }
    }
    
    //Compare two line in file
    if (n1 != n2 || strcmp(buffer_line_file_1, buffer_line_file_2) != 0) {
      isDiff = true;
    }
    
    //Immediately return if found diff in quiet mode
    if (isDiff == true && quiet == 1) {
      printf("diff: files differ\n");
      if (fd1 != 0) close(fd1);
      if (fd2 != 0) close(fd2);
      return;
    }

    //Scenario if two file the same
    if (is_EOF_1 == true && is_EOF_2 == true) {
      break; 
    }
    else if (is_EOF_1 == true) {
      printf("f1: %d: < EOF", lineNo);
      printf("f2: %d: < %s", lineNo, buffer_line_file_2);
    }
    else if (is_EOF_2 == true) {
      printf("f1: %d: < %s", lineNo, buffer_line_file_1);
      printf("f2: %d: < EOF", lineNo);
    }
    else if (isDiff == true) {
      printf("f1: %d: < %s", lineNo, buffer_line_file_1);
      printf("f2: %d: < %s", lineNo, buffer_line_file_2);
    }
    lineNo++;
  }

  if (fd1 != 0) close(fd1);
  if (fd2 != 0) close(fd2);

}

int
main(int argc, char *argv[])
{

  if (argc == 4 && strcmp(argv[1], "-q") == 0) {
    //diff -q f1 f2
    diff(argv[2], argv[3], 1);
     
  } else if (argc == 3) {
    //diff f1 f2
    diff(argv[1], argv[2], 10);
  }
  else {
    //Error
    printf("Usage: diff [-q] file1 file2\n");
    exit(1);
  }

  exit(0);
}
