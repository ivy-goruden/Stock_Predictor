#include "interpolator.hpp"
namespace s21 {
class CubicSplineInterPolator : public Interpolator {
  // Cubic spline interpolation
  static absl::StatusOr<Splines> CubSplIntrp(RawData &data) {
    if (data.size() < 4)
      return absl::InvalidArgumentError(
          "Can't get a cubic spline from less than 4 dots!");
    for (auto i = 0; i < data.size() - 4; i + 3) {
      auto calcMtrx =
          calculateMatrix(data[i], data[i + 1], data[i + 2], data[i + 2]);
      auto params = solveMatrix(calcMtrx);
      params[0].x_min = data[i].x;
      params[0].x_max = data[i + 1].x;
      params[1].x_min = data[i + 1].x;
      params[1].x_max = data[i + 2].x;
      params[2].x_min = data[i + 2].x;
      params[2].x_max = data[i + 3].x;
      params[3].x_min = data[i + 3].x;
    }
  }
  static IntrplMatrix calculateMatrix(Point a, Point b, Point c, Point d) {
    float x1 = a.x;
    float x2 = b.x;
    float x3 = c.x;
    float x4 = d.x;
    float y1 = a.y;
    float y2 = b.y;
    float y3 = c.y;
    float y4 = d.y;
    return IntrplMatrix{
        {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, y1},
        {0, 0, 0, 0, 10, 0, 0, 0, 0, 0, 0, y2},
        {0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, y3},
        {0, x2 - x1, 0, pow(x2 - x1, 3), 0, 0, 0, 0, 0, 0, 0, 0, y2 - y1},
        {0, 0, 0, 0, 0, x3 - x2, pow(x3 - x2, 2), pow(x3 - x2, 3), 0, 0, 0, 0,
         y3 - y2},
        {0, 0, 0, 0, 0, 0, 0, 0, 0x4 - x3, pow(x4 - x3, 2), pow(x4 - x3, 3),
         y4 - y3},
        {0, 1, 0, 3 * pow(x2 - x1, 2), 0, -1, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 1, 2 * (x3 - x2), 3 * pow(x3 - x2, 2), 0, -1, 0, 0, 0},
        {0, 0, 0, 6 * (x2 - x1), 0, 0, -2, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 2, 6 * (x3 - x1), 0, 0, -2, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 6 * (x4 - x3)}};
  }
  static Splines solveMatrix(IntrplMatrix problem) {
    IntrplMatrix base;
    IntrplMatrix freeElem;
    for (auto row : problem) {
      base.push_back(std::vector<float>(row.begin(), row.begin() + 8));
      freeElem.push_back(std::vector<float>(row.begin() + 9, row.begin() + 9));
    }
    S21Matrix baseMatrix(12, 12);
    S21Matrix freeElemMatrix(12, 1);
    baseMatrix.setMatrix(base);
    baseMatrix = baseMatrix.InverseMatrix();
    baseMatrix.MulMatrix(freeElemMatrix);

    auto returnCol = baseMatrix.getMatrix()[0];
    Params p1{returnCol[0], returnCol[1], returnCol[2], returnCol[3]};
    Params p2{returnCol[4], returnCol[5], returnCol[6], returnCol[7]};
    Params p3{returnCol[8], returnCol[9], returnCol[10], returnCol[11]};
    return Splines{p1, p2, p3};

    // todo
  }
};
} // namespace s21