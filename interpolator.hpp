#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include <vector>
namespace s21 {
class Interpolator {
  struct Params {
    float a, b, c;
  };
  struct Result {
    std::vector<Params> splines;
  };
  struct Point {
    float x, y;
  };
  using IntrplMatrix = std::vector<std::vector<float>>;
  using RawData = std::vector<Point>;
  // Cubic spline interpolation
  static absl::StatusOr<Result> CubSplIntrp(RawData &data) {
    if (data.size() < 3)
      return absl::InvalidArgumentError(
          "Can't get a cubic spline from less than 3 dots!");
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
        {x2 - x1, pow(x2 - x1, 3), 0, 0, 0, 0, 0, 0, y2 - y1},
        {0, 0, x3 - x2, pow(x3 - x2, 2), pow(x3 - x2, 3), 0, 0, 0, y3 - y2},
        {0, 0, 0, 0, 0, x4 - x3, pow(x4 - x3, 2), pow(x4 - x3, 3), y4 - y3},
        {1, 3 * pow(x2 - x1, 2), -1, 0, 0, 0, 0, 0, 0},
        {0, 0, 1, 2 * (x3 - x2), 3 * pow(x3 - x2, 2), -1, 0, 0, 0},
        {0, 6 * (x2 - x1), 0, -2, 0, 0, 0, 0, 0},
        {0, 0, 0, 2, 6 * (x3 - x1), 0, -2, 0, 0},
        {0, 0, 0, 0, 0, 0, 2, 6 * (x4 - x3), 0}};
  }
  static Result solveMatrix(IntrplMatrix) {
    
    // todo
  }
};
} // namespace s21