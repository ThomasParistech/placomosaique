/*********************************************************************************************************************
 * File : capsule_extraction_pattern.h                                                                               *
 *                                                                                                                   *
 * 2020 Thomas Rouch                                                                                                 *
 *********************************************************************************************************************/

#ifndef CAPSULE_EXTRACTION_PATTERN_H
#define CAPSULE_EXTRACTION_PATTERN_H

#include <vector>
#include <opencv2/core/mat.hpp>

/// @brief Class representing an orthogonal grid of capsules, and extracts cutouts of the capsules when the user
/// gives the class a warped 2D observation of this 2D grid in the 3D world.
///
/// @note The capsules images are saved in the output directory
class CapsuleExtractionPattern
{
public:
    /// @brief Sets the geometry of the real world capsules pattern
    ///
    ///  +----------------+---------------------------------------------+----------------+
    ///  |                |                                             |                |
    ///  |        edge_y  |                                             |  edge_y        |
    ///  |                v                                             v                |
    ///  |              XXXXX              ^   XXXXX                  XXXXX              |
    ///  |   edge_x    XXXXXXX        2 *  |  XXXXXXX                XXXXXXX    edge_x   |
    ///  +------------>XXXXXXX      radius |  XXXXXXX                XXXXXXX<------------+
    ///  |              XXXXX              v   XXXXX                  XXXXX              |
    ///  |                                    <----->                                    |
    ///  |                                    2*radius                                   |
    ///  |                                                                               |    height
    ///  |                                                                               |
    ///  |                                                                               |
    ///  |              XXXXX                  XXXXX                  XXXXX              |
    ///  |   edge_x    XXXXXXX                XXXXXXX                XXXXXXX    edge_x   |
    ///  +------------>XXXXXXX                XXXXXXX                XXXXXXX<------------+
    ///  |              XXXXX                  XXXXX                  XXXXX              |
    ///  |                ^                                             ^                |
    ///  |        edge_y  |                                             |  edge_y        |
    ///  |                |                                             |                |
    ///  +----------------+---------------------------------------------+----------------+
    ///
    ///                                     width
    ///
    ///
    /// @note Since the class only focuses on the geometry, the scale factor doesn't really matter and we can pass
    /// the arguments in any unit that suits us. However, @p radius will determine the export size in pixels of the
    /// extracted capsules. As a conclusion, it's mandatory to convert the unit into pseudo-pixels.
    /// @param width Width in pixels of the outer rectangular contour
    /// @param height Width in pixels of the outer rectangular contour
    /// @param edge_x Distance in pixels between the left edge and the first circles column
    /// @param edge_y Distance in pixels between the top edge and the first circles row
    /// @param n_cols Number of circles columns in the grid
    /// @param n_rows Number of circles rows in the grid
    /// @param radius Radius in pixels of the grid circles. It will determine the export size in pixels of the
    /// extracted capsules
    CapsuleExtractionPattern(int width,
                             int height,
                             int edge_x,
                             int edge_y,
                             int n_cols,
                             int n_rows,
                             int radius);

    /// @brief Maps the 2D detection of the 4 corners to our reference rectangular contour and extracts capsules on it
    /// using the geometry information
    /// @note The capsules images are saved in the output directory
    /// @param corners 4 points of the rectangle detected on the image
    /// @param src_img Image on which the rectangle has been detected
    /// @param output_rectified_image Detected ROI after the affine transformation, that makes it rectangular
    /// @param draw_circles Draw circles on @p output_rectified_image to show where capsules have been extracted
    bool warp_image_and_extract_capsules(const std::vector<cv::Point2f> &corners,
                                         const cv::Mat &src_img,
                                         cv::Mat &output_rectified_image,
                                         bool draw_circles = true);

private:
    size_t next_capsule_id_; ///< Next ID to be assigned

    int width_;
    int height_;
    int n_cols_;
    int n_rows_;
    int radius_;

    std::vector<std::vector<cv::Point2f>> grid_; ///< 2D grid containing the position of the center of each circle
    std::vector<cv::Point2f> refcorners_;        ///< 4 corners of the rectangle
    cv::Mat_<double> H_;                         ///< Homography

    cv::Mat capsule_;      ///< Tmp image use to store a capsule
    cv::Mat capsule_mask_; ///< Mask of the same size of the capsules. Used to crop them into disks

    const std::string output_directory_ = "/tmp/Capsules/";
};

#endif // CAPSULE_EXTRACTION_PATTERN_H
