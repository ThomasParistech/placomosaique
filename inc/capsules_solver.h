/*********************************************************************************************************************
 * File : capsules_solver.h                                                                                          *
 *                                                                                                                   *
 * 2020 Thomas Rouch                                                                                                 *
 *********************************************************************************************************************/

#ifndef CAPSULES_SOLVER_H
#define CAPSULES_SOLVER_H

#include <opencv2/core/mat.hpp>
#include <opencv2/core/cvstd.hpp>

class CircleGridPattern;

class CapsulesSolver
{
public:
    CapsulesSolver();

    /// @brief Makes a composition out of reference capsules to mimic the input image @p img
    /// @param img Input image
    /// @param capsules_dir Path to the directory containing the reference capsules
    /// @param n_rows Number of capsules rows of the final composition
    bool solve(const cv::Mat &img, const std::string &capsules_dir, int n_rows);

private:
    /// @brief Calls the class CircleGridPattern to extract circle cutouts from the image
    /// and displays them
    /// @param circle_grid Class used to extract the circle cutouts
    /// @param img Input image from which we want to extract cutouts
    /// @param output_cutouts Circular sub-images extracted from @p img
    /// @return true if it was successful
    bool extract_and_display_cutouts(const CircleGridPattern &circle_grid, const cv::Mat &img,
                                     std::vector<cv::Mat> &output_cutouts);

    /// @brief Loads reference capsules and compare them to the cutouts of the input image
    /// @param ref_capsules_paths Paths to the reference capsules
    /// @param cutouts Cutouts of the original image
    /// @param output_errors Error matrix representing the difference scores between reference capsules and cutouts
    /// of the input image. Coefficient [i][j]: score between a reference capsule i and a location j in the image.
    /// The lower the score the better
    /// @return true if it was successful
    bool compute_errors_matrix(const std::vector<cv::String> &ref_capsules_paths,
                               const std::vector<cv::Mat> &cutouts,
                               std::vector<std::vector<double>> &output_errors);

    /// @brief Multithreaded version of the method @ref compute_errors_matrix
    bool compute_errors_matrix_multithreaded(const std::vector<cv::String> &ref_capsules_paths,
                                             const std::vector<cv::Mat> &cutouts,
                                             std::vector<std::vector<double>> &output_errors);
};

#endif // CAPSULES_SOLVER_H