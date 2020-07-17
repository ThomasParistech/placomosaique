/*********************************************************************************************************************
 * File : circle_grid_pattern.cpp                                                                                    *
 *                                                                                                                   *
 * 2020 Thomas Rouch                                                                                                 *
 *********************************************************************************************************************/

#include <iostream>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "circle_grid_pattern.h"

CircleGridPattern::CircleGridPattern(int width, int height, int n_rows)
{
    // Finds the optimal grid geometry
    const double a = 2 + std::sqrt(3) * (n_rows - 1.0);           // a
    const double radius_0 = height / a;                           // R0
    const double n_cols_0_float = 0.5 * (width / radius_0 - 1.0); // N
    const int n_cols_0 = static_cast<int>(n_cols_0_float);        // N0
    const double res_n_cols_0 = n_cols_0_float - n_cols_0;        // Res0

    n_rows_ = n_rows;

    if (res_n_cols_0 < 0.5)
    {
        n_cols_ = n_cols_0;
        radius_ = radius_0;
    }
    else
    {
        n_cols_ = n_cols_0 + 1;
        radius_ = static_cast<double>(width) / (2 * n_cols_ + 1);
    }
    grid_width_ = static_cast<int>(radius_ * (2 * n_cols_ + 1));
    grid_height_ = static_cast<int>(radius_ * a);

    std::cout << "Grid Geometry: r = " << radius_
              << ", n_rows = " << n_rows_
              << ", n_cols = " << n_cols_
              << "." << std::endl;

    // Fills the grid with the position of the center of each circle
    double x_row;
    double y_row = radius_;
    const double x_step = 2 * radius_;
    const double y_step = std::sqrt(3) * radius_;
    grid_.reserve(n_rows_ * n_cols_);
    for (int i = 0; i < n_rows_; i++, y_row += y_step)
    {
        x_row = (i % 2 == 0 ? radius_ : 2 * radius_);
        for (int j = 0; j < n_cols_; j++, x_row += x_step)
            grid_.emplace_back(x_row, y_row);
    }

    // Initializes images used for cutout-extraction
    cutout_.create(2 * radius_, 2 * radius_, CV_8UC3);
    circular_mask_.create(2 * radius_, 2 * radius_, CV_8UC3);
    circular_mask_.setTo(0);
    cv::circle(circular_mask_, cv::Point2f(radius_, radius_), radius_, cv::Scalar::all(255), -1);
}

size_t CircleGridPattern::get_rows()
{
    return n_rows_;
}

size_t CircleGridPattern::get_cols()
{
    return n_cols_;
}

cv::Size CircleGridPattern::get_cutout_size()
{
    return cutout_.size();
}

bool CircleGridPattern::extract_cutouts(const cv::Mat &image, std::vector<cv::Mat> &output_cutouts) const
{
    if (image.rows < grid_height_ || image.cols < grid_width_)
    {
        std::cerr << "Wrong image size. Expected at least " << grid_height_ << "x" << grid_width_
                  << ". Got " << image.rows << "x" << image.cols << "." << std::endl;
        return false;
    }

    if (image.type() != CV_8UC3)
    {
        std::cerr << "Wrong image depth. Expected CV_8UC3. Got " << image.depth() << "." << std::endl;
        return false;
    }

    // Extract cutouts
    output_cutouts.resize(grid_.size());
    for (size_t i = 0; i < grid_.size(); i++)
    {
        cv::getRectSubPix(image, cutout_.size(), grid_[i], cutout_);
        output_cutouts[i] = cutout_ & circular_mask_;
    }
    return true;
}

bool CircleGridPattern::generate_image(const std::vector<cv::Mat> &sub_images, cv::Mat &output_image) const
{
    if (sub_images.size() != grid_.size())
    {
        std::cerr << "Wrong number of sub-images. Expected " << grid_.size() << "." << std::endl;
        return false;
    }

    output_image.create(grid_height_, grid_width_, CV_8UC3);
    output_image.setTo(cv::Scalar::all(0));
    cv::Mat roi;
    for (int i = 0; i < grid_.size(); i++)
    {
        cv::resize(sub_images[i], cutout_, cutout_.size());
        cv::Rect rect(grid_[i] - cv::Point2f(radius_, radius_), cutout_.size());
        roi = cv::Mat(output_image, rect);
        cutout_.copyTo(roi, circular_mask_);
    }
    return true;
}
