#include <iostream>
#include <vector>
#include <chrono>
#include <arm_neon.h>
#include <thread>
#include <cstdint>
#include <algorithm>
//use lazy, multithreading, vectorization(simd), bit shifting, memoization
//use int16 return type but int32 inside functions
struct mathFuncs {
  static int16_t q115(const double x) {
    long product = std::lround(x * 32768.0);
    // std::clamp didn't work, so I did std::min(std::max(prod, low), high)
    return static_cast<int16_t>(std::min(std::max(product, -32768L), 32767L));
  }
  inline int16_t lazyLog2(int16_t x) {
    return 0;
  }
  int16_t qMult(int16_t x, int16_t y) {
    return (int16_t)(((int32_t)x * y) >> 15);
  }
};
struct Matrix {
  private:
    std::vector<int16_t> listOfNums {};
  public:
    const size_t rows;
    const size_t cols;
    int16_t &operator[](const size_t ind) {
      return this->listOfNums[ind];
    }
    size_t size() const {
      return this->listOfNums.size();
    }
    Matrix(const std::vector<int16_t> &lof, const size_t r, const size_t c)
    : listOfNums(lof), rows(r), cols(c) {
      if (r * c != this->size()) {
        const std::string errMessage = "\noptai.cpp:" + std::to_string(__LINE__) + ": \033[31m\033[1merror\033[0m: "
        + "Cannot initialize type Matrix for invalid inputs: " + std::to_string(r) + ", "
        + std::to_string(c) + " for vector size " + std::to_string(this->size()) + " --- Process finished with exit code 1";
        throw std::out_of_range(errMessage);
      }
    }
    Matrix(const size_t r, const size_t c)
    : listOfNums(r * c, 0), rows(r), cols(c) {
        if (!(r && c)) {
          const std::string errMessage2 = "\n\033[1moptai.cpp\033[0m:" + std::to_string(__LINE__)
          + ":\033[31m\033[1m error:\033[0m invalid inputs " + std::to_string(r)
          + ", " + std::to_string(c) + " for given default initializer\n";
          throw std::out_of_range(errMessage2);
        }
      }
    int16_t indexMatrix(const unsigned int r, const unsigned int c) const {
      if (r > this->rows) {
        const std::string str = "Cannot index: input r: " + std::to_string(r) + " is greater than the amount of rows in Matrix m";
        throw std::out_of_range(str);
      }
      if (c > this->cols) {
        const std::string str = "Cannot index: input c: " + std::to_string(c) + " is greater than the amount of columns in Matrix m";
        throw std::out_of_range(str);
      }
      return this->listOfNums[r * cols + c];
    }

    //int16x8_t
    Matrix transpose() {
      std::vector<int16_t> newlof(this->rows * this->cols);
      auto remainder {this->size() % 8};
      for (int r{0}; r < this->rows - (this->rows % 8); r+=8) {
        for (int c{0}; c < this->cols - (this->cols % 8); c+=8) {
          int16x8_t v[8];
          for (int i{0}; i < 8; i++) {
            v[i] = vld1q_s16(&this->listOfNums[(r + i) * this->cols + c]);
          }
          // 16 bit passes; transposes with 2x2 blocks
          const int16x8_t load0_1 = vtrn1q_s16(v[0], v[1]); const int16x8_t load1_1 = vtrn2q_s16(v[0], v[1]);
          v[0] = load0_1; v[1] = load1_1;
          const int16x8_t load2_1 = vtrn1q_s16(v[2], v[3]); const int16x8_t load3_1 = vtrn2q_s16(v[2], v[3]);
          v[2] = load2_1; v[3] = load3_1;
          const int16x8_t load4_1 = vtrn1q_s16(v[4], v[5]); const int16x8_t load5_1 = vtrn2q_s16(v[4], v[5]);
          v[4] = load4_1; v[5] = load5_1;
          const int16x8_t load6_1 = vtrn1q_s16(v[6], v[7]); const int16x8_t load7_1 = vtrn2q_s16(v[6], v[7]);
          v[6] = load6_1; v[7] = load7_1;
          // 32 bit passes; transposes those 2x2 blocks with 4x4 blocks
          const int32x4_t load0_2 = vtrn1q_s32(vreinterpretq_s32_s16(v[0]), vreinterpretq_s32_s16(v[2]));
          const int32x4_t load1_2 = vtrn2q_s32(vreinterpretq_s32_s16(v[0]), vreinterpretq_s32_s16(v[2]));
          v[0] = vreinterpretq_s16_s32(load0_2); v[2] = vreinterpretq_s16_s32(load1_2);
          const int32x4_t load2_2 = vtrn1q_s32(vreinterpretq_s32_s16(v[1]), vreinterpretq_s32_s16(v[3]));
          const int32x4_t load3_2 = vtrn2q_s32(vreinterpretq_s32_s16(v[1]), vreinterpretq_s32_s16(v[3]));
          v[1] = vreinterpretq_s16_s32(load2_2); v[3] = vreinterpretq_s16_s32(load3_2);
          const int32x4_t load4_2 = vtrn1q_s32(vreinterpretq_s32_s16(v[4]), vreinterpretq_s32_s16(v[6]));
          const int32x4_t load5_2 = vtrn2q_s32(vreinterpretq_s32_s16(v[4]), vreinterpretq_s32_s16(v[6]));
          v[4] = vreinterpretq_s16_s32(load4_2); v[6] = vreinterpretq_s16_s32(load5_2);
          const int32x4_t load6_2 = vtrn1q_s32(vreinterpretq_s32_s16(v[5]), vreinterpretq_s32_s16(v[7]));
          const int32x4_t load7_2 = vtrn2q_s32(vreinterpretq_s32_s16(v[5]), vreinterpretq_s32_s16(v[7]));
          v[5] = vreinterpretq_s16_s32(load6_2); v[7] = vreinterpretq_s16_s32(load7_2);
          // 64 bit passes; transposes those 4x4 blocks with 8x8 blocks
          const int64x2_t load0_3 = vtrn1q_s64(vreinterpretq_s64_s16(v[0]), vreinterpretq_s64_s16(v[4]));
          const int64x2_t load1_3 = vtrn2q_s64(vreinterpretq_s64_s16(v[0]), vreinterpretq_s64_s16(v[4]));
          v[0] = vreinterpretq_s16_s64(load0_3); v[4] = vreinterpretq_s16_s64(load1_3);
          const int64x2_t load2_3 = vtrn1q_s64(vreinterpretq_s64_s16(v[1]), vreinterpretq_s64_s16(v[5]));
          const int64x2_t load3_3 = vtrn2q_s64(vreinterpretq_s64_s16(v[1]), vreinterpretq_s64_s16(v[5]));
          v[1] = vreinterpretq_s16_s64(load2_3); v[5] = vreinterpretq_s16_s64(load3_3);
          const int64x2_t load4_3 = vtrn1q_s64(vreinterpretq_s64_s16(v[2]), vreinterpretq_s64_s16(v[6]));
          const int64x2_t load5_3 = vtrn2q_s64(vreinterpretq_s64_s16(v[2]), vreinterpretq_s64_s16(v[6]));
          v[2] = vreinterpretq_s16_s64(load4_3); v[6] = vreinterpretq_s16_s64(load5_3);
          const int64x2_t load6_3 = vtrn1q_s64(vreinterpretq_s64_s16(v[3]), vreinterpretq_s64_s16(v[7]));
          const int64x2_t load7_3 = vtrn2q_s64(vreinterpretq_s64_s16(v[3]), vreinterpretq_s64_s16(v[7]));
          v[3] = vreinterpretq_s16_s64(load6_3); v[7] = vreinterpretq_s16_s64(load7_3);
          for (int i{0}; i < 8; i++) {
            vst1q_s16(&newlof[(c + i) * this->rows + r], v[i]);
          }
        }
      }
      Matrix finMatr {newlof, this->cols, this->rows};
      return finMatr;
    }
};
int main() {
  Matrix matrTest {8, 8};
  for (short i{0}; i < 64; i++) {
    matrTest[i] = i;
  }
  Matrix test2 = matrTest.transpose();
  for (auto i{0}; i < test2.size(); i++) {
    if (i + 1 == test2.size()) {
      std::cout << test2[i];
    }
    else {
      std::cout << test2[i] << ", ";
    }
  }
  std::cout << '\n';
  return 0;
}
