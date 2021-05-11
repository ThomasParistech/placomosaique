/*********************************************************************************************************************
 * File : circle_grid.cpp                                                                                            *
 *                                                                                                                   *
 * 2020 Thomas Rouch                                                                                                 *
 *********************************************************************************************************************/

#include "circle_grid.h"

CircleGridParams get_grid_params_from_rows(int width, int height, int rows)
{
    const double a = 2 + std::sqrt(3) * (rows - 1.0);             // a
    const double radius_0 = height / a;                           // R0
    const double n_cols_0_float = 0.5 * (width / radius_0 - 1.0); // N
    const int n_cols_0 = static_cast<int>(n_cols_0_float);        // N0
    const double res_n_cols_0 = n_cols_0_float - n_cols_0;        // Res0

    CircleGridParams params;
    params.rows = rows;
    params.touch_bottom = res_n_cols_0 < 0.5;
    params.cols = (params.touch_bottom ? n_cols_0 : n_cols_0 + 1);

    return params;
}

CircleGridParams get_grid_params_from_cols(int width, int height, int cols)
{
    //
}
