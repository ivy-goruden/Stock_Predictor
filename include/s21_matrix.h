#ifndef HEADER_MATRIX
#define HEADER_MATRIX

#include <cstddef>
#include <stdexcept>
#include <vector>

class S21Matrix {
private:
  int rows = 0;
  int cols = 0;
  std::vector<std::vector<float>> matrix;

public:
  S21Matrix(int newrows = 3,
            int newcols = 3); // создает матрицу rows на cols(меняет размер уже
                              // созданной)
  void change_matrix_size(int newrows, int newcols); // изменяет размер матрицы
  ~S21Matrix(); // освобождает память, выделенную под матрицу
  void print(); // выводит содержимое матрицы в консоль
  S21Matrix(const S21Matrix &other);            // copy
  S21Matrix(S21Matrix &&other) noexcept;        // move
  int getRows();                                // get rows
  int getCols();                                // get cols
  std::vector<std::vector<float>> &getMatrix(); // get matrix
  const std::vector<std::vector<float>> &getMatrix() const;
  void setRows(int newrows); // set rows
  void setCols(int newcols); // set cols
  void setMatrix(const std::vector<std::vector<float>> &newmatrix);
  bool EqMatrix(const S21Matrix &other);
  bool operator==(const S21Matrix &other); // lvalue
  void operator=(const S21Matrix &other);  // lvalue
  template <typename T, size_t R, size_t C>
  std::vector<std::vector<float>> set_to_matrix(const T (&arr)[R][C]);
  void SumMatrix(const S21Matrix &other);
  S21Matrix operator+(const S21Matrix &other); // lvalue
  void SubMatrix(const S21Matrix &other);
  S21Matrix operator-(const S21Matrix &other); // lvalue
  void MulNumber(float num);
  S21Matrix operator*(float num);
  void MulMatrix(const S21Matrix &other);
  S21Matrix operator*(const S21Matrix &other);
  void operator+=(const S21Matrix &other);
  void operator-=(const S21Matrix &other);
  void operator*=(const S21Matrix &other);
  void operator*=(float num);
  float operator()(int i, int j) const;
  S21Matrix Transpose();
  S21Matrix CalcComplements();
  float get_matrix_minor(int row, int column);
  float Determinant();
  S21Matrix InverseMatrix();
};

template <typename T, size_t R, size_t C>
std::vector<std::vector<float>> S21Matrix::set_to_matrix(const T (&arr)[R][C]) {
  std::vector<std::vector<float>> new_matrix(R, std::vector<float>(C));
  for (size_t i = 0; i < R; ++i) {
    for (size_t j = 0; j < C; ++j) {
      new_matrix[i][j] = arr[i][j];
    }
  }
  return new_matrix;
}
#endif
