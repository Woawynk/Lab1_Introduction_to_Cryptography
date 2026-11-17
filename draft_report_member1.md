# BẢN THẢO BÁO CÁO - PHẦN NHIỆM VỤ CỦA THÀNH VIÊN 1
## Môn: Nhập môn Mã hóa Mật mã - Lab 01

---

# 1. HỆ THỐNG XỬ LÝ SỐ LỚN (BIG NUMBER SYSTEM)

## 1.1. Đặt vấn đề và Cấu trúc dữ liệu `uint512_t`

Trong các hệ mật mã khóa công khai như Diffie-Hellman hay RSA, kích thước khóa an toàn tối thiểu hiện nay thường từ 512 bit, 1024 bit đến 2048 bit. Tuy nhiên, các kiểu dữ liệu nguyên thủy trong ngôn ngữ C/C++ tiêu chuẩn (như `unsigned long long`) chỉ hỗ trợ tối đa 64 bit. Do đó, việc tự thiết kế một hệ thống xử lý số nguyên lớn đa độ chính xác (Multi-precision Integer) là yêu cầu tiên quyết.

Trong bài tập này, nhóm đã cài đặt cấu trúc số nguyên không dấu 512-bit `uint512_t`:

```cpp
struct uint512_t {
    uint64_t digits[8]; // 8 phần tử x 64 bit = 512 bit
    ...
};
```

### Cách tổ chức bộ nhớ (Endianness):
Cấu trúc sử dụng mô hình **Little-endian** ở cấp độ mảng:
- `digits[0]`: Chứa 64 bit có trọng số thấp nhất (Least Significant Word - LSW).
- `digits[7]`: Chứa 64 bit có trọng số cao nhất (Most Significant Word - MSW).
- Giá trị toán học của số nguyên $A \in \text{uint512\_t}$ được tính theo công thức:
  $$A = \sum_{i=0}^{7} \text{digits}[i] \cdot 2^{64i}$$

---

## 1.2. Các toán tử số học và thao tác bit cơ bản

### a. Toán tử so sánh (`==`, `!=`, `<`, `>`, `<=`, `>=`)
Để so sánh hai số nguyên lớn $A$ và $B$, thuật toán duyệt từ phần tử có trọng số cao nhất (`digits[7]`) xuống phần tử có trọng số thấp nhất (`digits[0]`):
- Nếu $\text{digits}[i] > B.\text{digits}[i] \implies A > B$.
- Nếu $\text{digits}[i] < B.\text{digits}[i] \implies A < B$.
- Nếu toàn bộ các cặp từ $i = 7 \to 0$ bằng nhau thì $A == B$.
- Độ phức tạp thời gian: $O(1)$ (tối đa 8 phép so sánh).

### b. Toán tử cộng đa độ chính xác (`+`)
Thực hiện cộng từng từ 64-bit từ `digits[0]` đến `digits[7]` kèm theo biến nhớ (*carry*):
```cpp
uint512_t operator+(const uint512_t &a, const uint512_t &b) {
    uint512_t res;
    uint128_t carry = 0;
    for (size_t i = 0; i < 8; i++) {
        uint128_t sum = (uint128_t)a.digits[i] + b.digits[i] + carry;
        res.digits[i] = (uint64_t)sum;
        carry = sum >> 64; // Carry là bit thứ 65
    }
    return res;
}
```
Nhờ tận dụng kiểu dữ liệu `__uint128_t` trong trình biên dịch C++14, phép cộng 64-bit có nhớ được thực hiện chính xác tuyệt đối mà không sợ tràn số cục bộ.

### c. Toán tử trừ đa độ chính xác (`-`)
Thực hiện trừ từng từ 64-bit kèm theo biến mượn (*borrow*):
```cpp
uint512_t operator-(const uint512_t &a, const uint512_t &b) {
    uint512_t res;
    uint128_t borrow = 0;
    for (size_t i = 0; i < 8; i++) {
        uint128_t a_val = a.digits[i];
        uint128_t b_val = (uint128_t)b.digits[i] + borrow;
        if (a_val >= b_val) {
            res.digits[i] = (uint64_t)(a_val - b_val);
            borrow = 0;
        } else {
            res.digits[i] = (uint64_t)(((uint128_t)1 << 64) + a_val - b_val);
            borrow = 1;
        }
    }
    return res;
}
```

### d. Các phép dịch bit và kiểm tra bit
- `get_bit(size_t i)`: Trả về giá trị bit thứ $i \in [0, 511]$ bằng cách xác định từ chứa bit `word = i / 64` và vị trí `bit = i % 64`.
- `shift_left_1(uint64_t carry_in)`: Dịch toàn bộ 512 bit sang trái 1 bit, nhận bit `carry_in` đưa vào bit 0 và trả về bit cao nhất bị tràn ra ngoài (bit thứ 512). Hàm này tối ưu hóa riêng cho các vòng lặp chia và nhân mô-đun.
- `bit_length()`: Xác định chỉ số của bit 1 cao nhất cộng 1 (tức số lượng bit có nghĩa thực sự), giúp rút ngắn số vòng lặp khi tính toán với số nhỏ hơn 512 bit.

---

## 1.3. Các giải thuật số học mô-đun (Modular Arithmetic)

Điểm cốt lõi của bài tập là: **Mọi biến trung gian phải luôn nằm trong phạm vi 512 bit**, không được phép để xảy ra tràn số khi nhân hai số 512-bit (vốn sẽ tạo ra số 1024-bit nếu nhân thông thường).

### a. Phép chia lấy dư `mod(x, y)`:
Thuật toán chia lấy dư sử dụng phương pháp dịch bit mô phỏng phép chia nhị phân (Binary Long Division / Quy tắc Horner):
- Khởi tạo phần dư `rem = 0`.
- Duyệt từng bit của $x$ từ MSB về LSB:
  $$\text{rem} = (\text{rem} \ll 1) + x_i$$
  Nếu $\text{rem} \ge y$ thì $\text{rem} = \text{rem} - y$.
- **Cơ chế chống tràn:** Trước mỗi bước dịch bit, ta luôn đảm bảo $\text{rem} < y$. Sau khi dịch trái 1 bit và cộng $x_i$, giá trị mới tối đa là $2(y - 1) + 1 = 2y - 1$. Ngay sau đó, nếu giá trị $\ge y$ thì trừ đi $y$, đưa $\text{rem}$ trở lại khoảng $[0, y-1]$. Vì vậy, $\text{rem}$ không bao giờ vượt quá $y$ và hoàn toàn nằm gọn trong 512 bit.

### b. Phép cộng mô-đun `AddMod(x, y, p)`:
Để tính $(x + y) \pmod p$ mà không làm tràn số 512-bit khi $x + y \ge 2^{512}$:
- Ta có điều kiện tiền xử lý: $x < p$ và $y < p$.
- Xét điều kiện: Nếu $x \ge p - y$ (tương đương $x + y \ge p$):
  $$(x + y) \pmod p = (x + y) - p = x - (p - y)$$
- Vì $y < p \implies p - y > 0$ (phép trừ $p - y$ không bị âm).
- Vì $x \ge p - y \implies x - (p - y) \ge 0$ (phép trừ tiếp theo cũng không bị âm).
- Ngược lại, nếu $x < p - y \implies x + y < p < 2^{512}$ (phép cộng $x + y$ chắc chắn không tràn 512 bit).

### c. Phép nhân mô-đun `MultMod(x, y, p)`:
Nhân hai số 512-bit thông thường sẽ tạo ra kết quả 1024-bit. Để tránh tràn số, chương trình áp dụng thuật toán **Cộng và Nhân đôi (Double-and-Add)**:
- Ta biểu diễn $y = \sum_{i=0}^{n-1} y_i \cdot 2^i$ với $y_i \in \{0, 1\}$.
- Khi đó:
  $$x \cdot y \pmod p = \sum_{i=0}^{n-1} \left( y_i \cdot (x \cdot 2^i \pmod p) \right) \pmod p$$
- Giải thuật:
  1. Khởi tạo `ret = 0`, `cur_x = x % p`.
  2. Duyệt từng bit $y_i$ từ $i = 0 \to n-1$:
     - Nếu $y_i == 1$: `ret = AddMod(ret, cur_x, p)`.
     - Cập nhật lũy kế: `cur_x = AddMod(cur_x, cur_x, p)` (tương đương nhân đôi $2 \cdot \text{cur\_x} \pmod p$).
  3. Trả về `ret`.
- **Đánh giá:** Mỗi bước chỉ gọi hàm `AddMod`, do đó toàn bộ các giá trị trung gian đều $< p < 2^{512}$.

### d. Phép lũy thừa mô-đun `modular_exponentiation(base, exponent, p)`:
Tính $(\text{base}^{\text{exponent}}) \pmod p$ bằng thuật toán **Bình phương và Nhân (Square-and-Multiply)**:
- Ta biểu diễn số mũ $\text{exponent} = \sum_{i=0}^{n-1} e_i \cdot 2^i$.
- Khi đó:
  $$\text{base}^{\text{exponent}} \pmod p = \prod_{i=0}^{n-1} \left( \text{base}^{2^i} \right)^{e_i} \pmod p$$
- Giải thuật:
  1. Khởi tạo `ret = 1`, `cur_base = base % p`.
  2. Duyệt từng bit $e_i$ từ $i = 0 \to n-1$:
     - Nếu $e_i == 1$: `ret = MultMod(ret, cur_base, p)`.
     - Cập nhật bình phương: `cur_base = MultMod(cur_base, cur_base, p)`.
  3. Trả về `ret`.
- **Độ phức tạp:** $O(n^2)$ phép toán bit nhờ kết hợp Square-and-Multiply với Double-and-Add, thực thi một phép lũy thừa 512-bit trong khoảng vài chục mili-giây.

---

# 5. MÔ TẢ CHƯƠNG TRÌNH ĐÃ CÀI ĐẶT

## 5.1. Cấu trúc thư mục mã nguồn
```text
project_01_source/
├── u512.h / u512.cpp            # Khai báo và hiện thực cấu trúc số lớn uint512_t
├── modular_math.h / .cpp        # Hiện thực mod, AddMod, MultMod, modular_exponentiation
├── test_member1.cpp            # Bộ kiểm thử đơn vị cho tầng tính toán số lớn
├── Makefile                     # File tự động hóa biên dịch với g++
└── lib/fips202/                # Thư viện chuẩn mật mã băm SHAKE-128 (Keccak)
```

## 5.2. Hướng dẫn biên dịch và kiểm thử
Chương trình tuân thủ hoàn toàn tiêu chuẩn **C++14**, không phụ thuộc vào bất kỳ thư viện ngoài nào (chỉ sử dụng C++ Standard Library).

### Lệnh biên dịch trên Linux / macOS / MSYS2:
```bash
# Di chuyển vào thư mục mã nguồn
cd project_01_source

# Biên dịch và chạy bộ kiểm thử đơn vị
make run_test
```

### Lệnh biên dịch thủ công bằng `g++`:
```bash
g++ -O3 -std=c++14 -Wall -Wextra -I. -I./lib/fips202 test_member1.cpp u512.cpp modular_math.cpp -o test_member1
./test_member1
```

## 5.3. Kết quả kiểm thử tính đúng đắn
Bộ kiểm thử `test_member1.cpp` đã thực hiện xác minh:
1. **Kiểm tra cộng trừ và dịch bit:** Xác minh khả năng xử lý cờ nhớ (carry) và mượn (borrow) xuyên suốt 8 phần tử 64-bit; nạp xuất đúng chuỗi Hexadecimal.
2. **Kiểm tra số học mô-đun:** Kiểm chứng với modulo số nguyên tố lớn $10^9 + 7$ và các số ngẫu nhiên.
3. **Kiểm tra Định lý Fermat nhỏ:** Tính $a^{p-1} \equiv 1 \pmod p$ với các số nguyên tố, đảm bảo thuật toán Square-and-Multiply hoạt động chính xác 100%.
