#include <vector>
#include "absl/status/status.h"
#include "absl/status/statusor.h"
namespace s21{
    class Interpolator{
        struct Params{
            float a,b,c;
        };
        struct Result{
            std::vector<Params> splines;
        };
        struct Point{
            float x, y;
        };
        using IntrplMatrix = std::vector<std::vector<float>>;  
        using RawData = std::vector<Point>;
        //Cubic spline interpolation
        static absl::StatusOr<Result> CubSplIntrp(RawData& data){
            if (data.size() < 3) return absl::InvalidArgumentError("Can't get a cubic spline from less than 3 dots!");

        }
        static IntrplMatrix calculateMatrix(Point a, Point b, Point c){
            //todo
        }
        static Result solveMatrix(IntrplMatrix){
            //todo
        }
        
    };
}