#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "kernel/fs.h"
#include "user/user.h"


/*
  Chương trình du: Minh hoạ duyệt cây thư mục và tính toán dung lượng (Disk Usage).

  Cú pháp:
    du [path] [-a] [-s]
  Mặc định path="."

  Các cờ (flags) hoạt động:
  - Cờ "-a" (all): in ra dung lượng của cả file lẫn thư mục.
  - Cờ "-s" (summary): chỉ in ra tổng dung lượng của thư mục/path gốc, không in các mục con.

  Cách thuật toán hoạt động (Đệ quy):
  1. Dùng fstat() để lấy thông tin (st.size, st.type) của file/thư mục.
  2. Nếu là FILE: trả về st.size.
  3. Nếu là THƯ MỤC: Duyệt qua các struct dirent bên trong nó, gọi đệ quy 
     xuống các mục con để cộng dồn dung lượng.
  4. Bỏ qua "." và ".." để tránh đệ quy vô hạn.
*/

// Khai báo biến toàn cục để lưu trạng thái của cờ (options)
int is_all_flag = 0;     // cờ -a
int is_summary_flag = 0; // cờ -s

// Hàm kiểm tra xem tên có phải là thư mục hiện tại "." hoặc thư mục cha ".." không
static int
is_dot_or_dotdot(const char *name)
{
  if (strcmp(name, ".") == 0 || strcmp(name, "..") == 0) {
    return 1;
  }
  return 0;
}

// Lấy tên chuỗi C chuẩn từ struct dirent
static void
name_from_dirent(char *out, struct dirent *de)
{
  memmove(out, de->name, DIRSIZ);
  out[DIRSIZ] = '\0';
}

// Hàm nối chuỗi: ghép đường dẫn cha và tên con (vd: "a" + "f1.txt" -> "a/f1.txt")
static int
join_path(char *out, int outsz, const char *parent, const char *child)
{
  int len_parent = strlen(parent);
  int len_child = strlen(child);
  int need_slash = 1;

  if (len_parent > 0 && parent[len_parent - 1] == '/') {
    need_slash = 0;
  }

  // Kiểm tra độ dài tránh tràn bộ đệm
  if (len_parent + need_slash + len_child + 1 > outsz) {
    return -1; 
  }

  memmove(out, parent, len_parent);
  if (need_slash) {
    out[len_parent] = '/';
  }
  memmove(out + len_parent + need_slash, child, len_child);
  out[len_parent + need_slash + len_child] = '\0';
  
  return 0;
}

// Hàm đệ quy
static long
calculate_du(const char *path)
{
  int fd;
  struct stat st;
  long total_size_bytes = 0;

  // Mở đường dẫn
  fd = open(path, O_RDONLY);
  if (fd < 0) {
    fprintf(2, "du: cannot open %s\n", path);
    return 0;
  }

  // Lấy thông tin stat của file/thư mục
  if (fstat(fd, &st) < 0) {
    fprintf(2, "du: cannot stat %s\n", path);
    close(fd);
    return 0;
  }

  // Case 1: Nếu là FILE bình thường
  if (st.type == T_FILE) {
    total_size_bytes = st.size;
    
    // Nếu có cờ -a (in cả file) VÀ không có cờ -s (summary) thì mới in ra
    if (is_all_flag && !is_summary_flag) {
      printf("%d\t%s\n", (int)total_size_bytes, path);
    }
    
    close(fd);
    return total_size_bytes;
  }

  // Case 2: Nếu là THƯ MỤC
  if (st.type == T_DIR) {
    struct dirent de;
    total_size_bytes = 0; // Thư mục rỗng thì dung lượng là 0

    // Đọc từng entry bên trong thư mục
    while (1) {
      int n = read(fd, &de, sizeof(de));
      if (n <= 0) {
        break; // Hết entry hoặc lỗi
      }

      if (de.inum == 0) {
        continue; // Bỏ qua entry trống
      }

      char child_name[DIRSIZ + 1];
      name_from_dirent(child_name, &de);

      // Tránh đệ quy vô hạn
      if (is_dot_or_dotdot(child_name)) {
        continue;
      }

      // Tạo đường dẫn hoàn chỉnh tới mục con
      char child_path_buffer[512];
      if (join_path(child_path_buffer, sizeof(child_path_buffer), path, child_name) < 0) {
        fprintf(2, "du: path too long: %s/%s\n", path, child_name);
        continue;
      }

      // ĐỆ QUY: Tính dung lượng mục con và cộng dồn
      total_size_bytes += calculate_du(child_path_buffer);
    }
    
    close(fd);

    // In ra dung lượng thư mục hiện tại (nếu không bật chế độ summary)
    if (!is_summary_flag) {
      printf("%d\t%s\n", (int)total_size_bytes, path);
    }

    return total_size_bytes;
  }

  close(fd);
  return 0;
}

int
main(int argc, char *argv[])
{
  char *target_path = "."; // Mặc định là thư mục hiện tại

  // Duyệt qua các tham số dòng lệnh để gán cờ
  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "-a") == 0) {
      is_all_flag = 1;
    } 
    else if (strcmp(argv[i], "-s") == 0) {
      is_summary_flag = 1;
    } 
    else {
      // Nếu không phải cờ thì đó chính là path cần tính dung lượng
      target_path = argv[i];
    }
  }

  // Gọi hàm đệ quy tính toán
  long final_total_size = calculate_du(target_path);

  // Nếu người dùng yêu cầu in summary (-s), ta sẽ in ra kết quả cuối cùng ở đây
  if (is_summary_flag) {
    printf("%d\t%s\n", (int)final_total_size, target_path);
  }

  exit(0);
}