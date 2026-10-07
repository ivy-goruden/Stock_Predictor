#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "include/s21_matrix.h"
#include <vector>
namespace s21 {
class Interpolator {
public:
  struct Params {
    float a, b, c, d;
    float x_min = 0;
    float x_max = INT_MAX;
  };
  struct Point {
    float x, y;
  };
  using Splines = std::vector<Params>;
  using IntrplMatrix = std::vector<std::vector<float>>;
  using RawData = std::vector<Point>;

private:
  RawData data;

public:
  Interpolator(RawData d) : data(d) {};
  void loadData(RawData d) { data = d; }
  virtual RawData getGraphData(int dotsNum) = 0;
  virtual float getValue(float value) = 0;
};
} // namespace s21