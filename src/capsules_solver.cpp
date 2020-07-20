/*********************************************************************************************************************
 * File : capsules_solver.h                                                                                          *
 *                                                                                                                   *
 * 2020 Thomas Rouch                                                                                                 *
 *********************************************************************************************************************/

#include "capsules_solver.h"

CapsulesSolver::CapsulesSolver() {}

bool CapsulesSolver::solve(const cv::Mat &img, const std::string &capsules_dir, int n_rows)
{
    // Extract circle cutouts in the input image
    CircleGridPattern circle_grid(img.cols, img.rows, n_rows);
    std::vector<cv::Mat> cutouts;
    if (!extract_and_display_cutouts(circle_grid, img, cutouts))
        return false;

    // Find reference capsules paths
    std::vector<cv::String> ref_capsules_paths;
    cv::glob(capsules_dir + "/*.png", ref_capsules_paths);
    std::cout << "Found " << ref_capsules_paths.size() << " reference capsules." << std::endl;
    if (ref_capsules_paths.size() < cutouts.size())
    {
        std::cerr << "Not enough reference capsules. Needs at least " << cutouts.size() << "." << std::endl;
        return false;
    }

    // Compare the reference capsules to the cutouts of the input image
    std::cout << "Start comparing images..." << std::endl;
    std::vector<std::vector<double>> errors;
    if (!compute_errors_matrix(ref_capsules_paths, cutouts, errors))
    {
        std::cerr << "Failed" << std::endl;
        return false;
    }
    std::cout << "Done" << std::endl;

    // Solve
    std::cout << "Start finding the optimal matches..." << std::endl;
    GaleShapleyAlgorithm algo;
    std::vector<size_t> matches;
    if (!algo.solve(errors, matches))
    {
        std::cerr << "Failed" << std::endl;
        return false;
    }
    std::cout << "Done" << std::endl;

    // Display solution
    std::vector<cv::Mat> optim_capsules;
    optim_capsules.resize(cutouts.size());
    for (size_t i = 0; i < cutouts.size(); i++)
    {
        const int j = matches[i];
        optim_capsules[i] = cv::imread(ref_capsules_paths[j]);
    }
    cv::Mat optim_display;
    circle_grid.generate_image(optim_capsules, optim_display);
    cv::imwrite("/tmp/CapsulesImage.png", optim_display);
    cv::imshow("Optimal Solution", optim_display);
    cv::waitKey();
}

bool CapsulesSolver::extract_and_display_cutouts(const CircleGridPattern &circle_grid,
                                                 const cv::Mat &img,
                                                 std::vector<cv::Mat> &output_cutouts)
{
    output_cutouts.clear();
    if (!circle_grid.extract_cutouts(img, output_cutouts))
        return false;
    cv::Mat circles_img;
    if (!circle_grid.generate_image(output_cutouts, circles_img))
        return false;
    cv::imshow("Input image", circles_img);
    cv::waitKey();
    cv::destroyAllWindows();
    return true;
}

bool CapsulesSolver::compute_errors_matrix(const std::vector<cv::String> &ref_capsules_paths,
                                           const std::vector<cv::Mat> &cutouts,
                                           std::vector<std::vector<double>> &output_errors)
{
    output_errors.reserve(ref_capsules_paths.size());
    cv::Mat ref_caps;
    double output_error;
    for (const auto &ref_path : ref_capsules_paths)
    {
        output_errors.emplace_back();
        ref_caps = cv::imread(ref_path);
        cv::Scalar ref_mean = cv::mean(ref_caps);
        for (const auto &cutout : cutouts)
        {
            const cv::Scalar diff_means = ref_mean - cv::mean(cutout);
            const double output_error = std::sqrt(diff_means[0] * diff_means[0] + diff_means[1] * diff_means[1] + diff_means[2] * diff_means[2]);
            output_errors.back().emplace_back(output_error);
        }
    }
    return true;
}
