#include <iostream>
#include "src/aiMath.cpp"
#include <vector>
Activation av;
double dv_sig(const double &x) {
    return 1.0 - av.sig(x);
}
struct Matrix;
struct lazyFuncs {
  int i = 0;  
};
struct Matrix {
    private:
        std::vector<double> listOfNums {};
        int rows;
        int cols;
    public:
        static auto validateLength(const Matrix &m) -> bool {
            if ((m.rows * m.cols) != m.listOfNums.size()) {
                return false;
            }
            return true;
        }
        Matrix(const int roow, const int cool) 
            : rows(roow), cols(cool) {
            this->rows = roow;
            this->cols = cool;
            std::vector<double> list(roow * cool, 0);
            this->listOfNums = list;
        }
        Matrix(const int ro, const int co, const std::vector<double> &lis)
            : rows(ro), cols(co), listOfNums(ro * co, 0.0){
           this->rows = ro;
            this->cols = co;
            this->listOfNums = lis;
        }
        static Matrix errorholder() {
            return {0,0,{1}};
        }
        Matrix scalar(const int scalar) {
            try {
                if (!validateLength(*this)) {
                    throw std::make_tuple(this->listOfNums.size(), this->cols, this->rows);
                }
                else {
                    for (int i{0}; i < listOfNums.size(); ++i) {
                        listOfNums[i] *= scalar;
                    }
                return *this;
                }
            }
            catch (const std::tuple<size_t, int, int>& tup) {
                const size_t size {std::get<0>(tup)};
                const int c {std::get<1>(tup)};
                const int r {std::get<2>(tup)};
                std::cout << "Error: Invalid matrix length: " << size << " for input of " << r
                << "x" << c << " matrix --- exit code ";
            }
            return errorholder();
        };
        
        static auto indexMatrix(const Matrix &x, const int row, const int col) -> int {
            if (!(row > 0 && col > 0)) {
                std::cout << "Input must be greater than zero, uses 1-based indexing.\n";
                return 0;
            }
            if (!(row <= x.rows && col <= x.cols)) {
                std::cout << "Uses 1-based indexing.\n";
                return 0;
            }
            return x.listOfNums[(row - 1) * x.cols + (col - 1)];
        }
        friend std::ostream& operator<<(std::ostream &os, const Matrix &m) {
            os << "[";
            for (int i{0}; i < m.listOfNums.size(); ++i){
                if (i + 1 >= m.listOfNums.size()) {
                    os << m.listOfNums[i];
                }
                else {
                    os << m.listOfNums[i] << ",";
                }
            }
            os << "]";
            return os;
        }
        static int cheapTranspose(const Matrix &m, const int row, const int col) {
            return (col - 1) * m.rows + (row - 1);
        }
        static Matrix transpose(const Matrix &m) {
            if (!(validateLength(m))) {
                std::cout << "Error: R x C != Matrix list length --- exit code ";
                return errorholder();
            }
            int newrows {m.cols};
            int newcols {m.rows};
            std::vector<double> newlistOfNums(newrows * newcols);
            int currRow{0};
            int currCol{0};
            int ind{0};
            for (int i{0}; i < m.listOfNums.size(); i++) {
                currRow = i / m.cols;
                currCol = i % m.cols;
                ind = cheapTranspose(m, currRow + 1, currCol + 1);
                newlistOfNums[ind] = m.listOfNums[i];
            }
            Matrix finalreturn {newrows, newcols, newlistOfNums};
            return finalreturn;
        }
		static Matrix matrixMult(const Matrix &m, const Matrix &x) {
            if (m.cols != x.rows) {
                std::cout << "Error: Matrix1 column length != Matrix2 row length --- exit code ";
                return errorholder();
            }
            if (!(validateLength(m)) || !(validateLength(x))) {
                std::cout << "Error: R x C != Matrix list length --- exit code ";
                return errorholder();        
            }
            else {
                double runningTotal {0};
                Matrix multiplicator {transpose(x)};
                std::vector<double> tempList {};
                for (int i{1}; i < m.rows + 1; i++) {
                    for (int u{1}; u < multiplicator.rows + 1; u++) {
                        for (int z{0}; z < m.cols; z++) {
                            runningTotal += indexMatrix(multiplicator, u, z + 1) * indexMatrix(m, i, 1 + z);
                        }
                        tempList.push_back(runningTotal);
                        runningTotal = 0;
                    }
                }
                Matrix finalMatr {m.rows, x.cols, tempList};
                return finalMatr;
            }
        }
        static Matrix lazyMult(const Matrix &m, const Matrix &x) {
           return errorholder(); 
        }
};
int main() {
    Matrix y {2, 2, {1,2,3,4}};
    Matrix x {2, 3, {5,6,7,8,9,10}};
    std::cout << Matrix::matrixMult(y,x)  << '\n';
    std::cout << Matrix::transpose(y) << '\n';
    std::cout << y.scalar(2) << '\n';
    std::cout << Matrix::transpose(y) << '\n';
    Matrix z {2, 4};
    std::cout << z << '\n';
    return 0;
}
