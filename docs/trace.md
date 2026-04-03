# Syscall trace trong xv6

## I. Yêu Cầu

Thêm system call trace để điều khiển theo dõi các syscall bằng bitmask.

trace nhận đối số bitmask (số nguyên), bit ở vị trí nào bật thì theo dõi syscall có số định danh trong kernel tương ứng vị trí bit đó.

Khi syscall được theo dõi thì in ra màn hình: pid của tiến trình gọi, tên syscall, giá trị trả về của syscall.

trace chỉ bật theo dõi cho tiến trình gọi nó và các tiến trình con được fork sau đó, không ảnh hưởng các tiến trình khác.

Viết chương trình người dùng trace để chạy một chương trình khác với chế độ theo dõi đã bật.

## II. Ý tưởng

Ý tưởng tổng quát là quản lý trạng thái theo dõi theo từng tiến trình, thay vì theo toàn hệ thống.

Thêm thuộc tính trace_mask vào cấu trúc dữ liệu của tiến trình để nhớ bitmask biểu diễn nhóm syscall cần theo dõi.

Viết syscall trace lưu bitmask từ chương trình người dùng vào trace_mask của tiến trình gọi nó.

Tại hàm xử lý syscall tập trung, kiểm tra trace_mask của tiến trình gọi nó với số định danh trong kernel của syscall đang được gọi để quyết định có in thông tin hay không.

Duy trì tính kế thừa qua fork để tiến trình con giữ cùng giá trị trace_mask với tiến trình cha.

Cách làm này giúp cơ chế trace đơn giản, linh hoạt và không ảnh hưởng các tiến trình không liên quan.

## III. Thiết lập hạ tầng và Cấu trúc dữ liệu

Phần này tập trung vào các chỉnh sửa mang tính khai báo, ánh xạ, và cấu trúc dữ liệu.

### 1) user/user.h

Khai báo hàm trace() để chương trình người dùng có thể gọi.

### 2) user/usys.pl

Tạo mã mồi (stub) để chuyển lệnh gọi hàm thành lệnh hệ thống (ecall) với số định danh tương ứng.

### 3) kernel/syscall.h

Cấp số định danh cho syscall trace để user và kernel thống nhất ánh xạ.

### 4) kernel/proc.h

Thêm thuộc tính trace_mask vào struct proc để lưu bitmask theo dõi cho từng tiến trình.

### 5) kernel/proc.c

Khởi tạo trace_mask về o (mặc định không theo dõi) và kế thừa trace_mask từ tiến trình cha sang tiến trình con.

### 6) kernel/syscall.c

Ánh xạ số định danh tới hàm xử lý (sys_trace).

Thêm bảng tên các syscall.

Chèn logic kiểm tra trace_mask vào hàm xử lý syscall tập trung để in thông tin ra màn hình.

## IV. Thuật Toán

### 1. Luồng hoạt động (flowchart high-level)

trace.c gọi trace(bitmask)
-> kernel ghi bitmask vào trace_mask của tiến trình hiện tại
-> chương trình đích chạy và phát sinh syscall
-> kernel xử lý syscall tại điểm tập trung
-> đối chiếu số định danh syscall với trace_mask
-> nếu bit bật thì in: pid, tên syscall, giá trị trả về

Nhánh fork (tách và nhập lại vào flow chính):

Tách nhánh tại bước: "kernel xử lý syscall tại điểm tập trung", khi syscall hiện tại là fork
-> tạo tiến trình con
-> sao chép trace_mask từ cha sang con
-> cha và con tiếp tục chạy
-> nhập lại tại bước: "chương trình phát sinh syscall" trong flow chính
-> từ đây cả hai tiến trình đều đi qua cùng luồng kiểm tra/in trace như nhau

### 2. Các bước thực hiện (dry-run cụ thể)

Ví dụ dry-run với ý tưởng lệnh dạng: trace mask command args

1. Người dùng chạy trace.c với bitmask và lệnh đích.

2. Trong main của trace.c:

- parse bitmask từ argv.
- gọi trace(bitmask).

3. Từ user vào kernel:

- trace đi qua syscall stub (từ usys.pl sinh ra) để phát sinh ecall.
- dispatcher syscall định tuyến đến sys_trace.

4. Trong sys_trace (sysproc.c):

- đọc tham số bitmask bằng argint.
- gán vào myproc()->trace_mask.
- trả về thành công.

5. Quay lại trace.c:

- chuẩn bị nargv cho lệnh đích.
- gọi exec để thay thế tiến trình hiện tại bằng chương trình cần chạy.

6. Khi chương trình đích gọi một syscall bất kỳ:

- syscall nhận mã số định danh num từ thanh ghi a7.
- syscall gọi handler tương ứng qua bảng syscalls.
- kết quả trả về đặt ở thanh ghi a0.

7. Ngay sau khi có giá trị trả về:

- syscall kiểm tra bit num trong trace_mask của tiến trình hiện tại.
- nếu bật thì in ra màn hình pid, tên syscall, giá trị trả về của syscall.
- nếu tắt thì không in gì cả.

8. Trường hợp chương trình đích tạo tiến trình con bằng fork:

- fork sao chép trace_mask từ cha sang con, vì vậy tiến trình con cũng tiếp tục luồng kiểm tra syscall như tiến trình cha.

## V. Khó khăn và Giải pháp

### 1. Thực thi dòng lệnh sau trace bitmask

**Khó khăn:** Sau khi gọi trace(bitmask), cần chạy chương trình đích với các đối số của nó.

**Giải pháp:** Sử dụng syscall `exec()` để thay thế tiến trình hiện tại bằng chương trình cần chạy, giữ nguyên trace_mask đã được thiết lập.

### 2. Ánh xạ vị trí bit của trace_mask với số định danh của syscall

**Khó khăn:** Cần xác định bit nào trong trace_mask ứng với syscall nào để quyết định có theo dõi hay không.

**Giải pháp:** Sử dụng phép toán bitwise AND: `(trace_mask & (1 << syscall_num))` để kiểm tra bit tại vị trí `syscall_num`. Nếu kết quả khác 0 thì bit bật, tức là cần theo dõi syscall này.

### 3. In tên của syscall đang được gọi

**Khó khăn:** Khi in thông tin trace, cần in tên con người có thể đọc được của syscall (ví dụ "fork", "read"), không phải chỉ số định danh.

**Giải pháp:** Tạo mảng tên syscall (bảng ánh xạ), trong đó index của mảng chính là số định danh syscall, giá trị tại index đó là chuỗi tên syscall. Khi cần in, truy cập `syscall_names[syscall_num]` để lấy tên tương ứng.
