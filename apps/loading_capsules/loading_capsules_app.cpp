/*********************************************************************************************************************
 * File : loading_capsules_app.cpp                                                                                   *
 *                                                                                                                   *
 * 2020 Thomas Rouch                                                                                                 *
 *********************************************************************************************************************/

#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
#include <timer.h>

#include <capsule_extractor.h>

namespace boost_po = boost::program_options;
namespace fs = boost::filesystem;

struct Config
{
    std::string capsules_dir_path;
    bool display_caps = false;
};

/// @brief Utility function to parse command line attributes
bool parse_command_line(int argc, char *argv[], Config &config)
{
    std::cout << "Extract capsule cut-outs from pictures of grids of capsules."
              << std::endl
              << std::endl;

    boost_po::options_description options;
    // clang-format off
    options.add_options()
        ("help,h", "Produce help message.")
        ("input-capsules,i", boost_po::value<std::string>(&config.capsules_dir_path)->default_value("/tmp/Capsules"), "Path to the folder containing the pictures of the capsules grids.")
        ("display,d",        boost_po::bool_switch(&config.display_caps)->default_value(false), "Display the rectified capsules grid with circles showing where capsules have been extracted.")
        ;
    // clang-format on

    boost_po::variables_map vm;
    try
    {
        boost_po::store(boost_po::command_line_parser(argc, argv).options(options).run(), vm);
        boost_po::notify(vm);
    }
    catch (boost_po::error &e)
    {
        std::cerr << options << std::endl;
        std::cerr << "Parsing error:" << e.what() << std::endl;
        return false;
    }

    if (vm.count("help"))
    {
        std::cout << options << std::endl;
        return false;
    }

    if (!fs::exists(config.capsules_dir_path))
    {
        std::cerr << "The input capsules directory path doesn't exist: " << config.capsules_dir_path << std::endl;
        return false;
    }

    return true;
}

int main(int argc, char **argv)
{
    Config config;
    if (!parse_command_line(argc, argv, config))
        return 1;

    CapsuleExtractionPattern capsule_pattern(2160, 1630, 58, 20, 6, 5, 140);
    CapsuleExtractor extractor(capsule_pattern);
    {
        Timer timer("Extract and save capsules", Timer::MS);
        extractor.extract_capsules_from_directory(config.capsules_dir_path, config.display_caps);
    }
}
