#include "interpolator.hpp"
#include <cmath>
#include <stdexcept>
#include <utility>
namespace s21 {
// Cubic spline interpolation
class CubicSplineInterPolator : public Interpolator {
    private:
        Splines splines_;
    public:
        static absl::StatusOr<CubicSplineInterPolator> Init(RawData &data){
            if (data.size() < 4)
            return absl::InvalidArgumentError(
                "Can't get a cubic spline from less than 4 dots!");
            return CubicSplineInterPolator(data);
        }
        absl::StatusOr<RawData> getGraphData(int dotsNum) override{
            if (dotsNum <= static_cast<int>(splines_.size())+1){
                return absl::InvalidArgumentError(
                "Need more dots than was in input data!");
            }
            RawData rd;
            auto minX = splines_.front().x_min;
            auto maxX = splines_.back().x_max;
            for (float i = minX; i<=maxX; i+=(maxX-minX)/dotsNum){
                rd.push_back(Point{i, getValue(i)});
            }
            return rd;

        };
        float getValue(float value) override{
            if(splines_.empty()) return 7;
            for (auto spline : splines_){
                if (value<= spline.x_max){
                    return spline.a + spline.b*(value - spline.x_min) + spline.c*pow(value - spline.x_min,2) + spline.d*pow(value - spline.x_min,3);
                }
            }
            return 3;
        };
        void loadData(RawData& d) override{
            getParams(d);
        };
    private:
        CubicSplineInterPolator(RawData &data){
            getParams(data);
        }
            
        void getParams(RawData &data) {
            for (auto i = 0; i < static_cast<int>(data.size()) - 3; i += 3) {
            auto calcMtrx =
                calculateMatrix(data[i], data[i + 1], data[i + 2], data[i + 3]);
            auto params = solveMatrix(calcMtrx, data[i + 1].x - data[i].x,
                                      data[i + 2].x - data[i + 1].x,
                                      data[i + 3].x - data[i + 2].x);
            params[0].x_min = data[i].x;
            params[0].x_max = data[i + 1].x;
            params[1].x_min = data[i + 1].x;
            params[1].x_max = data[i + 2].x;
            params[2].x_min = data[i + 2].x;
            params[2].x_max = data[i + 3].x;
            splines_.insert(splines_.end(), params.begin(), params.end());
            }
        }
        static IntrplMatrix calculateMatrix(Point a, Point b, Point c, Point d) {
            const float x[] = {a.x, b.x, c.x, d.x};
            const float y[] = {a.y, b.y, c.y, d.y};
            float h[] = {x[1] - x[0], x[2] - x[1], x[3] - x[2]};
            for (float interval : h) {
                if (!(interval > 0.0f)) {
                    throw std::invalid_argument(
                        "Spline x values must be strictly increasing");
                }
            }

            IntrplMatrix matrix(12, std::vector<float>(13, 0.0f));
            int row = 0;

            // Each interval uses A*h00 + B*h10 + C*h01 + D*h11.
            for (int interval = 0; interval < 3; ++interval) {
                const int offset = interval * 4;
                matrix[row][offset] = 1.0f;
                matrix[row++][12] = y[interval];
                matrix[row][offset + 2] = 1.0f;
                matrix[row++][12] = y[interval + 1];
            }

            for (int interval = 0; interval < 2; ++interval) {
                const int left = interval * 4;
                const int right = left + 4;
                matrix[row][left + 3] = 1.0f / h[interval];
                matrix[row][right + 1] = -1.0f / h[interval + 1];
                ++row;
            }

            for (int interval = 0; interval < 2; ++interval) {
                const int left = interval * 4;
                const int right = left + 4;
                const float leftScale = 1.0f / (h[interval] * h[interval]);
                const float rightScale =
                    1.0f / (h[interval + 1] * h[interval + 1]);
                matrix[row][left] = 6.0f * leftScale;
                matrix[row][left + 1] = 2.0f * leftScale;
                matrix[row][left + 2] = -6.0f * leftScale;
                matrix[row][left + 3] = 4.0f * leftScale;
                matrix[row][right] = 6.0f * rightScale;
                matrix[row][right + 1] = 4.0f * rightScale;
                matrix[row][right + 2] = -6.0f * rightScale;
                matrix[row][right + 3] = 2.0f * rightScale;
                ++row;
            }

            const float firstScale = 1.0f / (h[0] * h[0]);
            matrix[row][0] = -6.0f * firstScale;
            matrix[row][1] = -4.0f * firstScale;
            matrix[row][2] = 6.0f * firstScale;
            matrix[row++][3] = -2.0f * firstScale;

            const float lastScale = 1.0f / (h[2] * h[2]);
            matrix[row][8] = 6.0f * lastScale;
            matrix[row][9] = 2.0f * lastScale;
            matrix[row][10] = -6.0f * lastScale;
            matrix[row][11] = 4.0f * lastScale;
            return matrix;
        }
        static Splines solveMatrix(IntrplMatrix problem, float h1, float h2,
                       float h3) {
            constexpr int unknowns = 12;
            for (int column = 0; column < unknowns; ++column) {
                int pivot = column;
                for (int row = column + 1; row < unknowns; ++row) {
                    if (std::abs(problem[row][column]) >
                        std::abs(problem[pivot][column])) {
                        pivot = row;
                    }
                }
                if (std::abs(problem[pivot][column]) < 1e-7f) {
                    throw std::runtime_error("Hermite spline system is singular");
                }
                std::swap(problem[column], problem[pivot]);

                const float divisor = problem[column][column];
                for (int entry = column; entry <= unknowns; ++entry) {
                    problem[column][entry] /= divisor;
                }
                for (int row = 0; row < unknowns; ++row) {
                    if (row == column) continue;
                    const float factor = problem[row][column];
                    for (int entry = column; entry <= unknowns; ++entry) {
                        problem[row][entry] -= factor * problem[column][entry];
                    }
                }
            }

            const float widths[] = {h1, h2, h3};
            Splines splines;
            for (int interval = 0; interval < 3; ++interval) {
                const int offset = interval * 4;
                const float A = problem[offset][12];
                const float B = problem[offset + 1][12];
                const float C = problem[offset + 2][12];
                const float D = problem[offset + 3][12];
                const float width = widths[interval];
                const float widthSquared = width * width;
                const float widthCubed = widthSquared * width;
                splines.push_back(Params{
                    A,
                    B / width,
                    (-3.0f * A - 2.0f * B + 3.0f * C - D) / widthSquared,
                    (2.0f * A + B - 2.0f * C + D) / widthCubed});
            }
            return splines;
        }
};
} // namespace s21