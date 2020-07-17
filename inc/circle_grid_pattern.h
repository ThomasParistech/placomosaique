/*********************************************************************************************************************
 * File : circle_grid_pattern.h                                                                                      *
 *                                                                                                                   *
 * 2020 Thomas Rouch                                                                                                 *
 *********************************************************************************************************************/

#ifndef CIRCLE_GRID_PATTERN_H
#define CIRCLE_GRID_PATTERN_H

#include <vector>
#include <opencv2/core/mat.hpp>

/// @brief Class composing small images into a bigger one according to a grid composed of rows of circles one above
/// the other.
///
/// Since the grid defines the locations where to put the images, the class needs as many sub-images as the size of
/// the grid to render an image. Each of the sub-images is automatically resized, cropped into a disk and drawn on
/// top of a black background, at the position indicated by the index in the grid.
///
/// The odd rows are shifted by the width of half a circle, in order to have a dense structure.
class CircleGridPattern
{
public:
    /// @brief Finds the optimal number of columns and the optimal radius for the circles in the grid. Then fills
    /// the grid with the position of the center of each circle.
    ///
    /// @param width Width in pixels of the image on which to build the grid
    /// @param height Height in pixels of the image on which to build the grid
    /// @param n_rows Grid's number of rows
    CircleGridPattern(int width, int height, int n_rows);

    /// @brief Extracts cutouts from an image using the grid
    /// @param image Input image from which we want to extract cutouts
    /// @param output_cutouts Circular sub-images extracted from @p image
    /// @note The vector of images has been flattened and contains the rows one after the other. The first
    /// sub-image is the one at the top-left position
    bool extract_cutouts(const cv::Mat &image, std::vector<cv::Mat> &output_cutouts) const;

    /// @brief Resizes and applies a circular ROI on subimages from @p sub_images , fills the grid with them and draws
    /// it on @p output_image
    ///
    /// @param sub_images Input vector containing as many images as the size of the grid
    /// @note The vector of images must be flattened and contains the rows one after the other. The first
    /// sub-image is the one at the top-left position
    /// @param output_image Output image representing the grid filled with the subimages
    /// @return true if there aren't the right number of sub-images
    bool generate_image(const std::vector<cv::Mat> &sub_images, cv::Mat &output_image) const;

    /// @brief Gets the number of rows in the grid
    size_t get_rows();

    /// @brief Gets the number of columns in the grid
    size_t get_cols();

    /// @brief Gets the size of a cutout
    cv::Size get_cutout_size();

private:
    std::vector<cv::Point2f> grid_; ///< 2D grid containing the position of the center of each circle

    cv::Mat cutout_;        ///< Tmp image use to store a cutout
    cv::Mat circular_mask_; ///< Mask of the same size of the cutouts. Used to crop them into disks

    int n_cols_;      ///< Number of columns in the grid
    int n_rows_;      ///< Number of rows in the grid
    int grid_width_;  ///< Actual width in pixels of the grid
    int grid_height_; ///< Actual height in pixels of the grid
    double radius_;   ///< Radius of a circle in the grid
};

// (W, H) is the size of the image to fit.
//
// The pattern is described by following parameters:
//   - r      : radius of each circle
//   - n_cols : number of columns
//   - n_rows : number of rows
//
// The size (w,h) of the pattern is given by:
//   - w = r * (2*n_cols + 1)
//   - h = r * (2 + sqrt(3)*[n_rows-1])
//
//
/// We define the error to minimize as :
/// err = (1 - w/W)2 + (1 - h/H)2
//
///////////////////////////////////////////////////////////////////
/// 1) Assume the rows are perfectly fitting the edges
/// err = (1 - w/W)2 + 0
//
// a = 2 + sqrt(3)*(n_rows -1)
// R0 = H/a
//
// N = (W/R0 - 1) / 2
// N = N0 + Res0
//
// W = R0 * (2*N + 1)  and   w = R0 * (2*N0 + 1)
//
// By subtracting, we get:
// W - w = R0 * 2*(N-N0)
//       = 2 * R0 * Res0
//
// Thus, err_0 = (1 - w/W)2
//             = ([W - w] / W)2
//       err_0 = (2*R0*Res0 / W)2
//
// Let's define a score as the inverse of the square root of the error:
//   s = 1/sqrt(err)
//
// s_0 = 1 / (1 - w/W)
// s_0 = W / (2*R0*Res0)
//
///////////////////////////////////////////////////////////////////
/// 2) Assume the columns are perfectly fitting the edges
/// err =  0 + (1 - h/H)2
//
// For the value of n_cols, we use (N0 +1)
// We try fitting an additional column and see if it's better
// R1 = W / (2*[N0+1] + 1)
//
// h1 = R1 * a
//    = W/([2*N0 + 1] + 2) * a
//    = W/([2*N0 + 1] + 2) * H/R0
//    = W*H / ([2*N0 + 1] * R0 + 2*R0)
// h1 = W*H / (w + 2*R0)
//
// Thus, err_1 = (1 - h1/H)2
//             = (1 - W / [w + 2*R0])2
//       err_1 = ([w + 2*R0 - W] / [w + 2*R0])2
//
// 1/s_1 = 1 - W / (w + 2*R0)
//       = 1 - 1 / (w/W + 2*R0/W)
//       = 1 - 1 / (1-1/s_0 + 1/[Res0*s_0])
//       = 1 - s_0 / (s_0-1 + 1/Res0)
// 1/s_1 = (-1 + 1/Res0) / (s_0-1 + 1/Res0)
//
// By inverting the equation, we get:
//  s_1 = (s_0 + [-1 + 1/Res0]) / [-1 + 1/Res0]
//  s_1 = s_0 / [-1 + 1/Res0] + 1
//
// We define x = 1 / [-1 + 1/Res0]
//           x = Res0 / (1 - Res0)
// We get:
//  s_1 = s_0*x + 1
//
// s_0 and s_1 are positive values greater than 1
// If Res0 < 0.5, then x < 1, which means  s_1 < s_0, i.e. err_1 > err_0
// If Res0 > 0.5, then x > 1, which means  s_1 > s_0, i.e. err_1 < err_0
//
// As a conclusion, the value of Res0 tells us if we should adjust perfectly the columns or the rows.

#endif // CIRCLE_GRID_PATTERN_H
