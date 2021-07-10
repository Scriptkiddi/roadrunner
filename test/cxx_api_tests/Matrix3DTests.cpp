//
// Created by Ciaran on 08/07/2021.
//

#include "gtest/gtest.h"
#include "Matrix3D.h"

using namespace rr;

class Matrix3DTests : public ::testing::Test {
public:
    Matrix3DTests() = default;
};


TEST_F(Matrix3DTests, CheckSlice) {
    Matrix3D<double, double> matrix3D(
            {0.0, 1.0},
            {
                    // 0.0
                    {
                            {0.0, 1.0},
                            {0.0, 1.0}
                    },
                    // 1.0
                    {
                            {0.0, 2.0},
                            {0.0, 2.0}
                    }
            }
    );
    Matrix<double> actual =  matrix3D[0.0];
    Matrix<double> expected({
                                    {0.0, 1.0},
                                    {0.0, 1.0}
                            });
    bool passed = expected.almostEquals(actual, 1e-4);
    ASSERT_TRUE(passed);
}

TEST_F(Matrix3DTests, NumRows) {
    Matrix3D<double, double> matrix3D(
            {0.0, 1.0},
            {
                    // 0.0
                    {
                            {0.0, 1.0},
                            {0.0, 1.0},
                            {0.0, 1.0}
                    },
                    // 1.0
                    {
                            {0.0, 2.0},
                            {0.0, 2.0},
                            {0.0, 2.0}
                    }
            }
    );
    ASSERT_EQ(matrix3D.numRows(), 3);
}

TEST_F(Matrix3DTests, NumCols) {
    Matrix3D<double, double> matrix3D(
            {0.0, 1.0},
            {
                    // 0.0
                    {
                            {0.0, 1.0},
                            {0.0, 1.0},
                            {0.0, 1.0}
                    },
                    // 1.0
                    {
                            {0.0, 2.0},
                            {0.0, 2.0},
                            {0.0, 2.0}
                    }
            }
    );
    ASSERT_EQ(matrix3D.numCols(), 2);
}

TEST_F(Matrix3DTests, NumZ) {
    Matrix3D<double, double> matrix3D(
            {0.0, 1.0},
            {
                    // 0.0
                    {
                            {0.0, 1.0},
                            {0.0, 1.0},
                            {0.0, 1.0}
                    },
                    // 1.0
                    {
                            {0.0, 2.0},
                            {0.0, 2.0},
                            {0.0, 2.0}
                    }
            }
    );
    ASSERT_EQ(matrix3D.numZ(), 2);
}

TEST_F(Matrix3DTests, InsertEmptyCheckNumRows) {
    // needs to set x, y and z
    Matrix3D<int, double> m;
    m.insert(0, {
            {0, 1, 2},
            {3, 4, 5},
    });
    ASSERT_EQ(2, m.numRows());
}

TEST_F(Matrix3DTests, InsertEmptyCheckNumCols) {
    // needs to set x, y and z
    Matrix3D<int, double> m;
    m.insert(0, {
            {0, 1, 2},
            {3, 4, 5},
    });
    ASSERT_EQ(3, m.numCols());
}

TEST_F(Matrix3DTests, InsertEmptyCheckNumZ) {
    // needs to set x, y and z
    Matrix3D<int, double> m;
    m.insert(0, {
            {0, 1, 2},
            {3, 4, 5},
    });
    ASSERT_EQ(1, m.numZ());
}












