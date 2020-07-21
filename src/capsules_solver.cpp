/*********************************************************************************************************************
 * File : capsules_solver.h                                                                                          *
 *                                                                                                                   *
 * 2020 Thomas Rouch                                                                                                 *
 *********************************************************************************************************************/

#include <future>

#include "timer.h"
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
    std::vector<std::vector<double>> errors;
    {
        Timer timer("Compute difference scores", Timer::MS);
        std::cout << "Start comparing images..." << std::endl;
        if (!compute_errors_matrix_multithreaded(ref_capsules_paths, cutouts, errors))
        {
            std::cerr << "Failed" << std::endl;
            return false;
        }
    }
    std::cout << "Done" << std::endl;

    // Solve
    std::cout << "Start finding the optimal matches..." << std::endl;
    GaleShapleyAlgorithm algo;
    std::vector<size_t> matches;
    {
        Timer timer("Find the optimal matching", Timer::MS);
        if (!algo.solve(errors, matches))
        {
            std::cerr << "Failed" << std::endl;
            return false;
        }
    }
    std::cout << "Done" << std::endl;

    // Display solution
    std::cout << "Start generating the optimal image..." << std::endl;
    cv::Mat optim_display;
    {
        Timer timer("Generate optimal image", Timer::MS);
        std::vector<cv::Mat> optim_capsules;
        optim_capsules.resize(cutouts.size());
        for (size_t i = 0; i < cutouts.size(); i++)
        {
            const int j = matches[i];
            optim_capsules[i] = cv::imread(ref_capsules_paths[j]);
        }
        circle_grid.generate_image(optim_capsules, optim_display);
    }
    std::cout << "Done" << std::endl;
    cv::imwrite("/tmp/CapsulesImage.png", optim_display);
    cv::imshow("Optimal Solution", optim_display);
    cv::waitKey();

    // Show errors
    std::cout << "Start computing the error map..." << std::endl;
    cv::Mat error_map;
    {
        Timer timer("Compute the error map", Timer::MS);
        std::vector<double> final_errors;
        final_errors.reserve(cutouts.size());
        for (size_t i = 0; i < cutouts.size(); i++)
        {
            const int j = matches[i];
            final_errors.push_back(errors[j][i]);
        }
        auto it_minmax = std::minmax_element(final_errors.cbegin(), final_errors.cend());
        const double error_min = *(it_minmax.first);
        const double error_max = *(it_minmax.second);
        std::cout << error_min << " -> " << error_max << std::endl;
        const double alpha = 255.0 / (error_max - error_min);
        const double beta = -error_min / alpha;

        std::vector<cv::Mat> errors_cutouts;
        errors_cutouts.reserve(cutouts.size());
        for (const auto &err : final_errors)
        {
            const unsigned char scaled_idx = alpha * err + beta;
            cv::Mat grey(circle_grid.get_cutout_size(), CV_8U);
            grey.setTo(scaled_idx);
            cv::Mat color;
            cv::applyColorMap(grey, color, cv::ColormapTypes::COLORMAP_JET);
            errors_cutouts.emplace_back(color);
        }
        circle_grid.generate_image(errors_cutouts, error_map);
    }
    std::cout << "Done" << std::endl;
    cv::imwrite("/tmp/CapsulesImage_errors.png", error_map);
    cv::imshow("Error map", error_map);
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

    std::vector<cv::Scalar> cutouts_means;
    cutouts_means.reserve(cutouts.size());
    for (const auto &cutout : cutouts)
        cutouts_means.emplace_back(cv::mean(cutout));

    cv::Mat ref_caps;
    double output_error;
    for (const auto &ref_path : ref_capsules_paths)
    {
        output_errors.emplace_back();
        ref_caps = cv::imread(ref_path);
        cv::Scalar ref_mean = cv::mean(ref_caps);
        for (const auto &cutout_mean : cutouts_means)
        {
            const cv::Scalar diff_means = ref_mean - cv::mean(cutout_mean);
            const double output_error = std::sqrt(diff_means[0] * diff_means[0] + diff_means[1] * diff_means[1] + diff_means[2] * diff_means[2]);
            output_errors.back().emplace_back(output_error);
        }
    }
    return true;
}

bool CapsulesSolver::compute_errors_matrix_multithreaded(const std::vector<cv::String> &ref_capsules_paths,
                                                         const std::vector<cv::Mat> &cutouts,
                                                         std::vector<std::vector<double>> &output_errors)
{
    std::vector<cv::Scalar> cutouts_means;
    cutouts_means.reserve(cutouts.size());
    for (const auto &cutout : cutouts)
        cutouts_means.emplace_back(cv::mean(cutout));

    output_errors.resize(ref_capsules_paths.size());
    std::vector<std::future<void>> futures;
    std::mutex writing_mutex;

    int idx = 0;
    for (const auto &ref_path : ref_capsules_paths)
    {
        futures.push_back(std::async(std::launch::async,
                                     [&writing_mutex](std::reference_wrapper<std::vector<double>> errors,
                                                      std::reference_wrapper<const std::vector<cv::Scalar>> cutouts_means,
                                                      std::reference_wrapper<const cv::String> ref_caps_path) {
                                         const cv::Scalar ref_mean = cv::mean(cv::imread(ref_caps_path.get()));
                                         std::vector<double> errs;
                                         errs.reserve(cutouts_means.get().size());
                                         for (const auto &cutout_mean : cutouts_means.get())
                                         {
                                             const cv::Scalar diff_means = ref_mean - cutout_mean;
                                             const double output_error = std::sqrt(diff_means[0] * diff_means[0] + diff_means[1] * diff_means[1] + diff_means[2] * diff_means[2]);
                                             errs.emplace_back(output_error);
                                         }
                                         {
                                             std::lock_guard<std::mutex> lock(writing_mutex);
                                             std::swap(errs, errors.get());
                                         }
                                     },
                                     std::ref(output_errors[idx++]), std::cref(cutouts_means), std::cref(ref_path)));
    }

    futures.clear();
    return true;
}
