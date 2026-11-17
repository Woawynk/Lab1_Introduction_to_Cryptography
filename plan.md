# KẾ HOẠCH TỐC HÀNH 3 NGÀY (3-DAY SPRINT PLAN)
## Môn học: Nhập môn Mã hóa Mật mã - Bộ môn Công nghệ Tri thức
### Đề tài: SỐ NGUYÊN TỐ & TRAO ĐỔI KHÓA DIFFIE-HELLMAN

---

> [!WARNING]
> **THỜI GIAN KHẨN CẤP: CHỈ CÒN 3 NGÀY (Hôm nay, Ngày mai, Ngày mốt)**
> - Chiến lược: **Code song song với Viết báo cáo**, ưu tiên tính đúng đắn và khả năng biên dịch sạch sẽ (`c++14`, không lỗi runtime).
> - Sử dụng trực tiếp thuật toán từ `pseudocode-modular-operation.pdf` và khung sườn `prng_sample/`.

---

## 1. PHÂN CÔNG TỐC HÀNH CHO 2 THÀNH VIÊN

| Vai trò | Thành viên 1 (Core Số lớn & Số học) | Thành viên 2 (Mật mã & Giao thức DH) |
| :--- | :--- | :--- |
| **Phần Code chính** | - Hoàn thiện cấu trúc `uint512_t` (`+`, `-`, so sánh, `<< 1`, `>> 1`).<br>- Viết `AddMod`, `MultMod`, `modular_exponentiation` theo đúng mã giả.<br>- Hỗ trợ Thành viên 2 debug hiệu năng. | - Tích hợp `prng_next_u512()` & sinh `private_key` $[2, p-2]$.<br>- Viết sàng chia thử số nguyên tố nhỏ & hàm **Miller-Rabin**.<br>- Viết hàm sinh **Safe Prime 512-bit** & ráp luồng DH (`main.cpp`). |
| **Phần Báo cáo (Viết song song)** | - **Mục 1:** Hệ thống xử lý số lớn (Cấu trúc, toán tử, giải thuật tránh tràn số).<br>- **Mục 5:** Mô tả chương trình (Cách chạy, lệnh `make`, chỉnh số bit). | - **Mục 2:** Thuật toán kiểm tra số nguyên tố (Miller-Rabin).<br>- **Mục 3:** Số ngẫu nhiên (CSPRNG, Keccak/SHAKE128 - *điểm cộng*).<br>- **Mục 4:** Giao thức Diffie-Hellman & ứng dụng thực tế (TLS/SSH). |
| **Cùng làm** | - Đo benchmark thời gian chạy (tính lũy thừa, sinh số nguyên tố).<br>- Hoàn thiện **Mục 6:** Tự đánh giá ưu/nhược điểm.<br>- Ghép và xuất báo cáo sang PDF, đóng gói `<MSSV1-MSSV2>.zip`. |

---

## 2. LỊCH TRÌNH 3 NGÀY CHI TIẾT

### 🚀 NGÀY 1 (HÔM NAY): HOÀN THÀNH TOÀN BỘ CODE NỀN TẢNG

*Mục tiêu cuối ngày 1: Chạy được phép lũy thừa mô-đun `modular_exponentiation` và thuật toán Miller-Rabin.*

- **Thành viên 1:**
  - [x] Định nghĩa `uint512_t` với 8 phần tử `uint64_t`.
  - [x] Cài toán tử cộng `+`, trừ `-`, so sánh `<`, `==`.
  - [x] Cài phép dịch bit `<< 1` và lấy bit thứ $i$.
  - [x] Lắp ráp mã giả từ `pseudocode-modular-operation.pdf`:
    - `AddMod(x, y, p)`
    - `MultMod(x, y, p)`
    - `ExpMod(x, y, p)` (chính là hàm `modular_exponentiation`).
  - [x] Bắt đầu gõ nháp Báo cáo **Mục 1**.

- **Thành viên 2:**
  - [x] Viết hàm `prng_next_u512()` từ thư viện `lib/fips202/` có sẵn.
  - [x] Viết hàm sinh số lẻ ngẫu nhiên 512-bit (bật bit 511 và bit 0).
  - [x] Viết mảng gồm 100 số nguyên tố nhỏ đầu tiên để lọc nhanh hợp số (chia thử).
  - [x] Cài đặt kiểm tra nguyên tố **Miller-Rabin** (sử dụng hàm `modular_exponentiation` của Thành viên 1).
  - [x] Bắt đầu gõ nháp Báo cáo **Mục 3** (Số ngẫu nhiên) và **Mục 2** (Miller-Rabin).

---

### 🚀 NGÀY 2 (NGÀY MAI): TÍCH HỢP GIAO THỨC DH & VIẾT BÁO CÁO

*Mục tiêu cuối ngày 2: Toàn bộ code chạy thông suốt, Alice & Bob trao đổi khóa thành công, hoàn thành 80% báo cáo.*

- **Thành viên 2:**
  - [ ] Hoàn thành hàm `generate_safe_prime(bit_size)`:
    - Vòng lặp: sinh số lẻ ngẫu nhiên $q$, chia thử nhanh các số nguyên tố nhỏ.
    - Kiểm tra $q$ bằng Miller-Rabin; nếu $q$ nguyên tố, tính $p = 2q + 1$.
    - Kiểm tra $p$ bằng Miller-Rabin; nếu đúng thì dừng và trả về $p$.
  - [ ] Viết hàm `generate_private_key(p)` ngẫu nhiên trong $[2, p-2]$.
  - [ ] Viết hàm `main()` kết nối luồng Diffie-Hellman đầy đủ:
    - Sinh $p$ và chọn $g = 2$.
    - Sinh $a, b \rightarrow$ Tính $A = g^a \pmod p, B = g^b \pmod p$.
    - Tính $s_A = B^a \pmod p, s_B = A^b \pmod p$.
    - In ra màn hình xác nhận `s_A == s_B`.
  - [ ] Viết tiếp Báo cáo **Mục 4** (Diffie-Hellman và TLS/SSH).

- **Thành viên 1:**
  - [ ] Nhận code từ Thành viên 2, hỗ trợ kiểm thử và sửa lỗi logic/tràn số (nếu có).
  - [ ] Viết Makefile hoàn chỉnh, kiểm tra cờ biên dịch: `g++ -O3 -std=c++14 -Wall -Wextra`.
  - [ ] Hoàn thiện nội dung Báo cáo **Mục 1** và viết tiếp **Mục 5** (Hướng dẫn chạy & giải thích code).

---

### 🚀 NGÀY 3 (NGÀY MỐT): BENCHMARK, HOÀN THIỆN BÁO CÁO & ĐÓNG GÓI NỘP BÀI

*Mục tiêu cuối ngày 3: Có đầy đủ file ZIP theo đúng định dạng, sẵn sàng nộp bài trước hạn.*

- **Buổi sáng (Cả 2 cùng làm):**
  - [ ] Chạy đo thời gian thực thi (Benchmark) bằng `std::chrono`:
    - Thời gian thực hiện 1 phép lũy thừa mô-đun 512-bit.
    - Thời gian sinh 1 số nguyên tố an toàn (Safe Prime) 512-bit.
    - Ghi lại số liệu đưa vào báo cáo.
  - [ ] Viết **Mục 6 Báo cáo:** Tự đánh giá ưu/nhược điểm (Ví dụ: ưu điểm là thuật toán an toàn không tràn số; nhược điểm là phép nhân Double-and-Add có độ phức tạp $O(n^2)$ nên sinh safe prime mất vài giây đến vài chục giây; hướng cải thiện: áp dụng nhân Montgomery hoặc đa luồng).
- **Buổi chiều:**
  - [ ] Ghép toàn bộ nội dung thành 1 file báo cáo hoàn chỉnh (Word / Google Docs / LaTeX) và xuất sang **PDF** (`project_01_report/report.pdf`).
  - [ ] Kiểm tra lại mã nguồn: Xóa hết file rác, file `.o`, file nhị phân trong thư mục source.
- **Buổi tối (Trước hạn nộp):**
  - [ ] Đóng gói thư mục thành file ZIP chuẩn:
    ```text
    <MSSV1-MSSV2>.zip
    ├── project_01_source/
    │   ├── Makefile
    │   ├── main.cpp
    │   ├── u512.h / u512.cpp
    │   ├── dh.h / dh.cpp
    │   └── lib/fips202/
    └── project_01_report/
        └── report.pdf
    ```
  - [ ] Giải nén thử ra một thư mục khác, chạy lệnh `make` thử để đảm bảo 100% biên dịch không lỗi trên C++14.
  - [ ] Nộp bài lên hệ thống môn học.

---

## 3. CHECKLIST CỨU NGUY (ĐỂ CHẮC CHẮN ĐẠT ĐIỂM TỐI ĐA TRONG 3 NGÀY)

1. **Không bị 0 điểm:** Code **phải biên dịch được** (`make`). Tuyệt đối không để sót lỗi cú pháp lúc nộp.
2. **Không dùng thư viện cấm:** Chỉ dùng C++ STL chuẩn (`<iostream>`, `<cstdint>`, `<vector>`,...), không dùng thư viện ngoài như GMP, NTL.
3. **Mẹo tăng tốc sinh số nguyên tố an toàn (Cực kỳ quan trọng):**
   - Muốn sinh nhanh Safe Prime 512-bit, **bắt buộc phải có bước chia thử các số nguyên tố nhỏ (2, 3, 5, 7, 11, ...)** cho cả $q$ và $2q+1$ trước khi chạy Miller-Rabin. Bước này loại bỏ >80% hợp số tức thì, giúp giảm thời gian sinh từ vài phút xuống chỉ còn vài giây!
4. **Mục điểm cộng trong tầm tay:** Mục 3 (Số ngẫu nhiên CSPRNG) đã có sẵn thư viện Keccak/SHAKE128 trong đề tài; chỉ cần viết lý thuyết về nó là ăn trọn điểm cộng.
