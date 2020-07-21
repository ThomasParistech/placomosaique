/*********************************************************************************************************************
 * File : capsule_extraction_pattern.cpp                                                                             *
 *                                                                                                                   *
 * 2020 Thomas Rouch                                                                                                 *
 *********************************************************************************************************************/

#include <sstream>
#include <iostream>
#include <opencv2/calib3d.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <boost/filesystem.hpp>

#include "capsule_extraction_pattern.h"

namespace fs = boost::filesystem;

CapsuleExtractionPattern::CapsuleExtractionPattern(int width,
                                                   int height,
                                                   int edge_x,
                                                   int edge_y,
                                                   int n_cols,
                                                   int n_rows,
                                                   int radius) : width_(width),
                                                                 height_(height),
                                                                 n_cols_(n_cols),
                                                                 n_rows_(n_rows),
                                                                 radius_(radius),
                                                                 refcorners_(4),
                                                                 next_capsule_id_(0)

{

    // Top-Left, Top-Right, Bottom-Right, Bottom-Left (Clockwise)
    refcorners_[0] = cv::Point2f(0, 0);
    refcorners_[1] = cv::Point2f(width, 0);
    refcorners_[2] = cv::Point2f(width, height);
    refcorners_[3] = cv::Point2f(0, height);

    const double dwidth = double(width - 2 * n_cols * radius - 2 * edge_x) / (n_cols - 1);
    const double dheight = double(height - 2 * n_rows * radius - 2 * edge_y) / (n_rows - 1);
    const double step_x = dwidth + 2 * radius;
    const double step_y = dheight + 2 * radius;

    // Fills the 2D grid with the position of the center of the circles
    grid_.reserve(n_rows);
    double y = edge_y + radius;
    for (int i = 0; i < n_rows; i++, y += step_y)
    {
        grid_.emplace_back();
        auto &back = grid_.back();
        back.reserve(n_cols);
        double x = edge_x + radius;
        for (int j = 0; j < n_cols; j++, x += step_x)
            back.emplace_back(x, y);
    }

    // Initializes images used for capsule-cropping
    capsule_.create(2 * radius_, 2 * radius_, CV_8UC3);
    capsule_mask_.create(2 * radius_, 2 * radius_, CV_8U);
    capsule_mask_.setTo(0);
    cv::circle(capsule_mask_, cv::Point2f(radius_, radius_), radius_, cv::Scalar::all(255), -1);

    // Create output directory
    fs::remove_all(output_directory_);
    fs::create_directories(output_directory_);
}

bool CapsuleExtractionPattern::warp_image_and_extract_capsules(const size_t capsules_batch_id,
                                                               const std::vector<cv::Point2f> &corners,
                                                               const cv::Mat &src_img,
                                                               cv::Mat &output_rectified_image,
                                                               bool draw_circles)
{
    if (corners.size() != 4)
    {
        std::cerr << "Wrong number of corners. Expected 4." << std::endl;
        return false;
    }

    // Find the homography between the 4 observed corners and the reference ones
    H_ = cv::findHomography(corners, refcorners_, 0);

    // Warp the image to get only the pattern
    cv::warpPerspective(src_img, output_rectified_image, H_, cv::Size(width_, height_));

    // Extract and save cutouts
    int id = 0;
    for (const auto &row : grid_)
        for (const auto &pt : row)
        {
            cv::Mat roi(output_rectified_image, cv::Rect(pt.x - radius_, pt.y - radius_, radius_ * 2, radius_ * 2));
            roi.copyTo(capsule_, capsule_mask_);

            std::stringstream ss;
            ss << output_directory_ << "capsule_" << capsules_batch_id << "_" << id++ << ".png";
            cv::imwrite(ss.str(), capsule_);
        }

    // Draw circles around the capsules
    if (draw_circles)
    {
        for (const auto &row : grid_)
            for (const auto &pt : row)
                cv::circle(output_rectified_image, pt, radius_, cv::Scalar(0, 0, 255), 3);
    }
    return true;
}

int CapsuleExtractionPattern::get_number_of_capsules_per_image() const
{
    return n_cols_ * n_rows_;
}