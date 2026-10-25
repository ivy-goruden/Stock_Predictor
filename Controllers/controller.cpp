#include "intercontroller.hpp"
namespace s21{
    parsedGraph normalizeGraph(CubicSplineInterPolator::RawData points){
        if (points.empty()) return parsedGraph{};
        std::sort(points.begin(), points.end(),
            [](const Interpolator::Point& a, const Interpolator::Point& b){ return a.x < b.x; });
        const float dateOrigin = points.front().x;
        const float dateRange = points.back().x - dateOrigin;
        float maxVal = 0;
        if (dateRange <= 0) return parsedGraph{};
        for (auto& point : points){
            point.x = (point.x - dateOrigin) / dateRange;
            if (std::abs(point.y) > maxVal) {maxVal = std::abs(point.y);}
        }
        if (maxVal != 0){
            for (auto& point : points){
                point.y /= maxVal;
            }
        }
        else{
            maxVal = 1;
        }
        return parsedGraph{
            points, static_cast<long long>(dateOrigin), dateRange, maxVal
        };
    }

}