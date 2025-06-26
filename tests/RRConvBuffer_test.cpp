#include "gtest/gtest.h"
#include "WaveletFilter.hpp"
#include <cstdio>



TEST(RRConvBuffer, testConv) {
    RRConvBuffer cb;

    cb.config(db2_coeffs.length);
    cb.add(-0.011996224523f);
    ASSERT_FALSE(cb.is_ready_dc());
    cb.add(-0.006033323705f);
    ASSERT_TRUE(cb.is_ready_dc());
    float result = cb.conv(db2_coeffs.dc_lo);
    float expected = -0.0019081106f;
    EXPECT_FLOAT_EQ(result, expected);
    cb.add(-0.015806514770f);
    cb.add(-0.031758580357f);
    ASSERT_TRUE(cb.is_ready_dc());
    result = cb.conv(db2_coeffs.dc_lo);
    expected = -0.01027377582717134f;
    EXPECT_FLOAT_EQ(result, expected);
    cb.add(-0.033543430269f);
    cb.add(0.000434041023f);
    result = cb.conv(db2_coeffs.dc_lo);
    expected = -0.04177525392136743f;
    EXPECT_FLOAT_EQ(result, expected);

}

