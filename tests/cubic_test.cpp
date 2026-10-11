#include <gtest/gtest.h>
#include "../classes/CubicSplineInterpolator.hpp"
using Point = s21::Interpolator::Point;
TEST(cubicIntrp, firstTest) {
    s21::Interpolator::RawData data{Point{1,1}, Point{3,5}, Point{6,3}, {9,1}};
    absl::StatusOr<s21::CubicSplineInterPolator> cubIntrp = s21::CubicSplineInterPolator::Init(data);
    GTEST_ASSERT_TRUE(cubIntrp.ok());
    EXPECT_NEAR(cubIntrp.value().getValue(1), 1.0, 1e-5);
    EXPECT_NEAR(cubIntrp.value().getValue(2), 3.4324324, 1e-5);
    EXPECT_NEAR(cubIntrp.value().getValue(3), 5.0, 1e-5);
    EXPECT_NEAR(cubIntrp.value().getValue(6), 3.0, 1e-5);
    EXPECT_NEAR(cubIntrp.value().getValue(9), 1.0, 1e-5);
    auto graphDots = cubIntrp.value().getGraphData(6);
    for (auto& dot: graphDots.value()){
        std::cout<< dot.x << " " << dot.y << std::endl;
    }
    // GTEST_ASSERT_EQ();
    // EXPECT_FALSE(storage.set("alpha", 99));
    // ASSERT_TRUE(storage.set("beta", 20));
}