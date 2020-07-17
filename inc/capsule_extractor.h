/*********************************************************************************************************************
 * File : capsule_extractor.h                                                                                        *
 *                                                                                                                   *
 * 2020 Thomas Rouch                                                                                                 *
 *********************************************************************************************************************/

#ifndef CAPSULE_EXTRACTOR_H
#define CAPSULE_EXTRACTOR_H

#include "capsule_extraction_pattern.h"

/// @brief Class that processes pictures of capsules grids (warped 2D observations) and detects the contour of the grid
/// so that the class @ref CapsuleExtractionPattern can extract and save the cutouts of the capsules.
class CapsuleExtractor
{
public:
    /// @brief Constructor
    /// @param capsules_pattern Class extracting capsules from warped 2D observation of a capsules grid
    CapsuleExtractor(const CapsuleExtractionPattern &capsules_pattern);

    /// @brief Extracts capsules from a directory containing pictures of capsules grids (warped 2D observations)
    /// @note The cutouts of the capsules will then be saved
    /// @param input_dir Folder containing the pictures of the capsules grids
    /// @param display Display the rectified capsules grid with circles showing where capsules have been extracted
    void extract_capsules_from_directory(const std::string &input_dir, bool display = false);

private:
    /// @brief Extracts capsules from a picture of capsules grids (warped 2D observation)
    /// @note The cutouts of the capsules will then be saved
    /// @param input_img Picture of the capsules grid
    /// @param display Display the rectified capsules grid with circles showing where capsules have been extracted
    /// @return true if it was successful
    bool extract_capsules(const cv::Mat &input_img, bool display);

    /// @brief Finds largest contour in the image after applying a threshold on the grayscale intensity
    /// @note It aims at finding the warped contour of the rectangular capsules grid
    /// @param src_img Image on which to extract the contour
    /// @param output_contour Output vector containing the points of the contour
    /// @param display Display the thresholded input image
    /// @param ths Threshold used to binarize the image
    bool get_largest_contour(const cv::Mat &src_img, std::vector<cv::Point2f> &output_contour, bool display, int ths = 90);

    /// @brief Fits a quadrilateral to a contour
    /// @param input_contour Contour to fit
    /// @param output_quadrilateral 4 output points representing the optimal quadrilateral passing through the contour
    /// @note They're arranged clockwise: Top-Left, Top-Right, Bottom-Right, Bottom-Left
    /// @return true if it was successful
    bool fit_quadrilateral(const std::vector<cv::Point2f> &input_contour, std::vector<cv::Point2f> &output_quadrilateral);

    template <typename T, typename O>
    inline T clamp_val(T val, const O min, const O max)
    {
        return std::min(static_cast<T>(max), std::max(static_cast<T>(min), val));
    }

    CapsuleExtractionPattern capsules_pattern_; ///< Class extracting and saving capsules

    // Display
    cv::Mat resized_img_;
    cv::Mat drawing_img_;
    const int resized_height_ = 500; ///< Resize image before processing it

    // Contour
    cv::Mat src_gray_;
    cv::Mat ths_img_;
    std::vector<cv::Point2f> best_contour_;
    std::vector<cv::Point2f> quadrilateral_contour_;
    std::vector<std::vector<cv::Point>> contours_;
    std::vector<cv::Vec4i> hierarchy_;
};

#endif // CAPSULE_EXTRACTOR_H
