# Đồ Án 01: Số Nguyên Tố & Trao Đổi Khóa Diffie-Hellman

Môn học: **Nhập môn Mã hóa Mật mã** — Bộ môn Công nghệ Tri thức, Trường Đại học Khoa học Tự nhiên, ĐHQG-HCM.

---

## 1. Giới thiệu
Chương trình hiện thực hoàn chỉnh từ đầu (from scratch) cấu trúc số lớn 512-bit không dấu (`uint512_t`), các phép toán số học mô-đun an toàn không tràn số (`AddMod`, `MultMod`, `modular_exponentiation`), thuật toán kiểm tra số nguyên tố xác suất Miller-Rabin, sinh số nguyên tố an toàn (Safe Prime) $p = 2q + 1$, và giao thức trao đổi khóa Diffie-Hellman giữa hai bên Alice và Bob.

Chương trình tuân thủ tiêu chuẩn **C++14**, không sử dụng bất kỳ thư viện ngoài nào (như GMP hay NTL), hỗ trợ đa nền tảng (Linux & Windows).

---

## 2. Cấu trúc thư mục
```text
.
├── project_01_source/       # Mã nguồn C++ chính của đồ án
│   ├── u512.h / u512.cpp            # Cấu trúc uint512_t và các toán tử
│   ├── modular_math.h / .cpp        # mod, AddMod, MultMod, modular_exponentiation
│   ├── dh.h / dh.cpp                # Miller-Rabin, sinh Safe Prime, sinh khóa riêng
│   ├── main.cpp                     # Chương trình chính trao đổi khóa Diffie-Hellman
│   ├── test_suite.cpp              # Bộ kiểm thử số học, primality, g = 2 và DH
│   ├── Makefile                     # File tự động hóa biên dịch
│   └── lib/fips202/                # Bộ sinh giả ngẫu nhiên chuẩn mật mã SHAKE-128
└── project_01_report/       # Báo cáo đồ án
    ├── report.tex                   # Mã nguồn LaTeX báo cáo đầy đủ 6 mục
    └── make_run_test.png            # Ảnh minh chứng chạy nghiệm thu
```

---

## 3. Hướng dẫn biên dịch và chạy

### Yêu cầu môi trường
- Trình biên dịch C++ hỗ trợ chuẩn **C++14** (`g++`) và công cụ `make`.
- Trên Windows: Sử dụng môi trường `w64devkit` hoặc MinGW.
- Trên Linux: Sử dụng `build-essential`.

### Lệnh thực thi

```bash
cd project_01_source

# 1. Chạy bộ kiểm thử đơn vị tầng số lớn:
make run_test

# 2. Chạy toàn bộ chương trình trao đổi khóa Diffie-Hellman:
make run

# 3. Dọn dẹp các file thực thi và object:
make clean
```
