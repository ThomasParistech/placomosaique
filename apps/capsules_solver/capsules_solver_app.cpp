/*********************************************************************************************************************
 * File : capsules_solver_app.cpp                                                                                    *
 *                                                                                                                   *
 * 2020 Thomas Rouch                                                                                                 *
 *********************************************************************************************************************/

#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
#include <iostream>

#include <capsules_solver.h>

namespace boost_po = boost::program_options;
namespace fs = boost::filesystem;

struct Config
{
    cv::Mat input_img;
    std::string capsules_dir_path;
    int n_rows;

    bool display_errors;
    std::string output_dir_path;
};

/// @brief Utility function to parse command line attributes
bool parse_command_line(int argc, char *argv[], Config &config)
{
    std::string image_path;

    const std::string short_program_desc(
        "Find the optimal arrangement of champagne capsules to represent a given photograph.\n");

    const std::string long_program_desc(
        short_program_desc +
        "\nFirst, the app pauses and shows you the capsules grid superimposed on the input image, \n"
        "to see which part of the image will be used for the matching. This grid is composed of\n"
        "rows of circles one above the other and will be filled with champagne capsules once the \n"
        "optimisation is done.\n"
        "Press any key to continue.\n"
        "Then, the algorithms computes the optimal combination and displays its solution.\n"
        "If the option has been enabled, the error map is then displayed.\n");

    boost_po::options_description options_desc;
    boost_po::options_description base_options("Base options");
    // clang-format off
    base_options.add_options()
        ("help,h", "Produce help message.")
        ("input-image,i", boost_po::value<std::string>(&image_path), "Path to an image file.")
        ("input-capsules,c", boost_po::value<std::string>(&config.capsules_dir_path)->default_value("/tmp/Capsules"), "Path to the directory containing the loaded capsules.")
        ("nbr-rows,r", boost_po::value<int>(&config.n_rows), "Number of capsules rows of the final composition.")
        ;
    // clang-format on

    boost_po::options_description output_options("Output options");
    // clang-format off
    output_options.add_options()
        ("display-errors,e", boost_po::value(&config.display_errors)->default_value(false), "Activate computation and display of the error map or not.")
        ("out-dir,o", boost_po::value<std::string>(&config.output_dir_path)->default_value("/tmp/placomosaic"), "Path of the output directory used to save the images and generate an html "
                                                                       "grid listing the ids of the capsules used in the composition.")
        ;
    // clang-format on

    options_desc.add(base_options).add(output_options);

    boost_po::variables_map vm;
    try
    {
        boost_po::store(boost_po::command_line_parser(argc, argv).options(options_desc).run(), vm);
        boost_po::notify(vm);
    }
    catch (boost_po::error &e)
    {
        std::cerr << short_program_desc << std::endl;
        std::cerr << options_desc << std::endl;
        std::cerr << "Parsing error:" << e.what() << std::endl;
        return false;
    }

    if (vm.count("help"))
    {
        std::cout << short_program_desc << std::endl;
        std::cout << options_desc << std::endl;
        return false;
    }

    if (!fs::exists(config.output_dir_path))
    {
        try
        {
            fs::create_directories(config.output_dir_path);
        }
        catch (fs::filesystem_error &e)
        {
            std::cerr << "Unable to create folder" << config.output_dir_path << std::endl;
            return false;
        }
    }
    if (config.n_rows <= 0)
    {
        std::cerr << "The number of rows must be strictly positive. Got " << config.n_rows << "." << std::endl;
        return false;
    }
    if (!fs::exists(image_path))
    {
        std::cerr << "The input image path doesn't exist: " << image_path << std::endl;
        return false;
    }
    try
    {
        config.input_img = cv::imread(image_path);
    }
    catch (...)
    {
        std::cerr << "Fail to load the image from " << image_path << std::endl;
        return 1;
    }

    if (!fs::exists(config.capsules_dir_path))
    {
        std::cerr << "The input capsules directory path doesn't exist: " << config.capsules_dir_path << std::endl;
        return false;
    }

    std::cout << long_program_desc << std::endl;
    return true;
}

int main(int argc, char **argv)
{
    Config config;
    if (!parse_command_line(argc, argv, config))
        return 1;

    CapsulesSolver solver;
    solver.solve(config.input_img, config.capsules_dir_path, config.n_rows);
    return 0;
}
