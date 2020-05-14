/*********************************************************************************************************************
 * File : hungarian_logger.h                                                                                         *
 *                                                                                                                   *
 *                                                                                                                   *
 *                                                                                                                   *
 *********************************************************************************************************************/

#ifndef HUNGARIAN_LOGGER_H
#define HUNGARIAN_LOGGER_H

#include <vector>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>

/// @brief
class HungarianLogger
{
public:
    HungarianLogger(int n_rows);

    void draw_numbers(const std::vector<std::vector<double>> &grid);
    void cover_rows_and_cols(const std::vector<bool> &covered_rows,
                             const std::vector<bool> &covered_cols);
    void draw_zeros(const std::map<int, int> &selected_in_row, const std::map<int, int> &prepared_in_row);

    void draw_alternated_zeros_serie(const std::vector<std::pair<int, int>> &prepared_serie,
                                     const std::vector<std::pair<int, int>> &selected_serie);

    void imwrite();

private:
    struct Cell
    {
        Cell(int i_top, int j_left, int i_bottom, int j_right)
        {
            top_left_ = cv::Point(j_left, i_top);
            bottom_right_ = cv::Point(j_right, i_bottom);
            middle_ = 0.5 * (top_left_ + bottom_right_);
        }

        cv::Point top_left_;
        cv::Point middle_;
        cv::Point bottom_right_;
    };

    std::vector<std::pair<cv::Point, cv::Point>> grid_cols_;
    std::vector<std::pair<cv::Point, cv::Point>> grid_rows_;
    std::vector<std::vector<Cell>> cells_;

    int current_step_;

    int n_rows_;
    int step_size_ = 60;
    int display_size_;
    cv::Mat img_;

    cv::Scalar bg_color_ = cv::Scalar::all(255);
    cv::Scalar text_color_ = cv::Scalar::all(0);
    cv::Scalar grid_color_ = cv::Scalar::all(0);
    cv::Vec3b line_color_ = cv::Vec3b(0, 0, 255);

    cv::Vec3b selected_zero_color_ = cv::Vec3b(0, 255, 0);
    cv::Vec3b prepared_zero_color_ = cv::Vec3b(255, 0, 0);
    cv::Vec3b alternated_serie_color_ = cv::Vec3b(255, 0, 255);

    std::string output_dir_ = "/tmp/hungarian_steps";
    std::string output_filename_base_;
};

#endif // HUNGARIAN_LOGGER_H
