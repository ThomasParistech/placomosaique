/*********************************************************************************************************************
 * File : circle_grid.h                                                                                              *
 *                                                                                                                   *
 * 2020 Thomas Rouch                                                                                                 *
 *********************************************************************************************************************/

#ifndef CIRCLE_GRID_H
#define CIRCLE_GRID_H

#include <opencv2/core/mat.hpp>
#include <vector>

struct CircleGridParams
{
    int rows;
    int cols;
    bool touch_bottom;
};

CircleGridParams get_grid_params_from_rows(int width, int height, int rows);

CircleGridParams get_grid_params_from_cols(int width, int height, int cols);

void get_2d_grid(const CircleGridParams &params, std::vector<cv::Point2f> &output_grid)
{
    //
}

bool extract_cutouts(const CircleGridParams &params,
                     const cv::Mat &image,
                     std::vector<cv::Mat> &output_cutouts)
{
    get_2d_grid
}

bool generate_image(const CircleGridParams &params,
                    const std::vector<cv::Mat> &sub_images,
                    cv::Mat &output_image);

#endif // CIRCLE_GRID_H
