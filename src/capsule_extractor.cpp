/*********************************************************************************************************************
 * File : capsule_extractor.cpp                                                                                      *
 *                                                                                                                   *
 * 2020 Thomas Rouch                                                                                                 *
 *********************************************************************************************************************/

#include <iostream>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "capsule_extractor.h"

//CapsuleExtractionPattern(2160, 1630, 58, 20, 6, 5, 140));

CapsuleExtractor::CapsuleExtractor(const CapsuleExtractionPattern &capsules_pattern) : capsules_pattern_(capsules_pattern)
{
    n_capsules_per_image_ = capsules_pattern.get_number_of_capsules_per_image();
}

void CapsuleExtractor::extract_capsules_from_directory(const std::string &input_dir, bool display)
{
    std::vector<cv::String> filenames;
    cv::glob(input_dir + "/*.jpeg", filenames);

    // Iterate over the directory and try extracting capsules on each picture
    cv::Mat img;
    int n_capsules = 0;
    for (size_t i = 0; i < filenames.size(); i++)
    {
        img = cv::imread(filenames[i]);
        if (extract_capsules(i, img, display))
        {
            n_capsules += n_capsules_per_image_;
            std::cout << "Loaded " << n_capsules << " capsules" << std::endl;
        }
        else
            std::cerr << "Fail to extract from " << filenames[i] << std::endl;
    }
}

bool CapsuleExtractor::extract_capsules(const size_t capsules_batch_id, const cv::Mat &input_img, bool display)
{
    const int resized_width = (resized_height_ * input_img.cols) / input_img.rows;
    cv::resize(input_img, resized_img_, cv::Size(resized_width, resized_height_));

    if (!get_largest_contour(resized_img_, best_contour_, display))
        return false;

    if (!fit_quadrilateral(best_contour_, quadrilateral_contour_))
        return false;

    // Extract the capsules
    cv::Mat resized_rectified_img;
    capsules_pattern_.warp_image_and_extract_capsules(capsules_batch_id, quadrilateral_contour_, resized_img_, resized_rectified_img, true);

    // Show results
    if (display)
    { // Draw best 4-points contour
        resized_img_.copyTo(drawing_img_);
        for (int i = 0; i < 4; i++)
        {
            cv::circle(drawing_img_, quadrilateral_contour_[i], 4 * (i + 1), cv::Scalar(0, 0, 255), -1);
            cv::line(drawing_img_, quadrilateral_contour_[i], quadrilateral_contour_[(i + 1) % 4], cv::Scalar(0, 0, 255), 1, cv::LINE_AA);
        }

        cv::imshow("Fitted contour", drawing_img_);
        cv::imshow("Rectified", resized_rectified_img);
        cv::waitKey();
    }
    return true;
}

bool CapsuleExtractor::get_largest_contour(const cv::Mat &src_img,
                                           std::vector<cv::Point2f> &output_contour,
                                           bool display,
                                           int ths)
{
    // Convert it to gray
    cv::cvtColor(src_img, src_gray_, CV_BGR2GRAY);
    cv::threshold(src_gray_, ths_img_, ths, 255, cv::THRESH_BINARY_INV);

    if (display)
    {
        cv::imshow("Thresholded image", ths_img_);
        cv::waitKey();
    }

    // Find contours
    contours_.clear();
    hierarchy_.clear();
    cv::findContours(ths_img_, contours_, hierarchy_, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE,
                     cv::Point(0, 0));

    if (contours_.empty())
        return false;

    // Find the contour with the largest area
    std::sort(contours_.begin(), contours_.end(),
              [](const std::vector<cv::Point> &left, const std::vector<cv::Point> &right) {
                  return cv::contourArea(left) > cv::contourArea(right);
              });

    output_contour.clear();
    output_contour.insert(output_contour.end(), contours_[0].begin(), contours_[0].end());
    if (contours_.size() == 1)
        return true;

    // Make sure the first contour is way larger than the second one
    if (cv::contourArea(contours_[1]) < 0.1 * cv::contourArea(output_contour))
        return true;

    std::cerr << "Error: Contour isn't large enough." << std::endl;
    return false;
}

bool CapsuleExtractor::fit_quadrilateral(const std::vector<cv::Point2f> &input_contour,
                                         std::vector<cv::Point2f> &output_quadrilateral)
{
    const double epsilon = 0.1 * cv::arcLength(input_contour, true);
    std::vector<std::vector<cv::Point2f>> approx_polygon;
    approx_polygon.emplace_back();
    cv::approxPolyDP(input_contour, approx_polygon.back(), epsilon, true);

    if (approx_polygon[0].size() != 4)
    {
        std::cerr << "Error: Wrong number of points : " << approx_polygon[0].size() << std::endl;
        return false;
    }

    output_quadrilateral.clear();
    output_quadrilateral.insert(output_quadrilateral.end(), approx_polygon[0].begin(), approx_polygon[0].end());

    // Sort the 4-points clockwise, starting from Top-Left:
    // Top-Left, Top-Right, Bottom-Right, Bottom-Left

    // First put the 2 top points at the beginning of the vector
    std::sort(output_quadrilateral.begin(),
              output_quadrilateral.end(),
              [](const cv::Point2f &left, const cv::Point2f &right) {
                  return left.y < right.y;
              });
    auto &top_1 = output_quadrilateral[0];
    auto &top_2 = output_quadrilateral[1];
    auto &bot_1 = output_quadrilateral[2];
    auto &bot_2 = output_quadrilateral[3];

    // Top-Left, Top-Right
    if (top_1.x > top_2.x)
        std::swap(top_1, top_2);

    // Bottom-Right, Bottom-Left
    if (bot_1.x < bot_2.x)
        std::swap(bot_1, bot_2);

    return true;
}
