/*********************************************************************************************************************
 * File : hungarian_logger.cpp                                                                                       *
 *                                                                                                                   *
 *                                                                                                                   *
 *                                                                                                                   *
 *********************************************************************************************************************/
#include <sstream>
#include <boost/filesystem.hpp>
#include "hungarian_logger.h"

HungarianLogger::HungarianLogger(int n_rows) : n_rows_(n_rows), display_size_(step_size_ * n_rows_), current_step_(0)
{
    img_.create(display_size_, display_size_, CV_8UC3);

    boost::filesystem::remove_all(output_dir_);
    boost::filesystem::create_directories(output_dir_);
    std::stringstream ss;
    ss << output_dir_ << "/"
       << "step_";
    output_filename_base_ = ss.str();

    /// Precompute the lines composing the grid
    grid_cols_.reserve(n_rows_);
    grid_rows_.reserve(n_rows_);
    for (double i = 0; i < display_size_; i += step_size_)
    {
        const int int_i = static_cast<int>(i);
        /// Column int_i
        grid_cols_.emplace_back(std::make_pair(cv::Point(int_i, 0), cv::Point(int_i, display_size_)));
        /// Row int_i
        grid_rows_.emplace_back(std::make_pair(cv::Point(0, int_i), cv::Point(display_size_, int_i)));
    }

    /// Precompute the corner points of each cell
    cells_.reserve(n_rows);
    for (double i = 0; i < display_size_; i += step_size_)
    {
        cells_.emplace_back();
        auto &back = cells_.back();
        back.reserve(n_rows);
        for (double j = 0; j < display_size_; j += step_size_)
        {
            const int j_right = std::min(static_cast<int>(j + step_size_), display_size_ - 1);
            const int i_bottom = std::min(static_cast<int>(i + step_size_), display_size_ - 1);
            back.emplace_back(Cell(i, j, i_bottom, j_right));
        }
    }
}

void HungarianLogger::draw_numbers(const std::vector<std::vector<double>> &grid)
{
    /// Draw grid
    img_.setTo(bg_color_);
    for (const auto &col : grid_cols_)
        cv::line(img_, col.first, col.second, grid_color_);
    for (const auto &row : grid_rows_)
        cv::line(img_, row.first, row.second, grid_color_);

    /// Draw numbers
    for (int i = 0; i < n_rows_; i++)
    {
        for (int j = 0; j < n_rows_; j++)
        {
            std::stringstream ss;
            ss << grid[i][j];
            cv::putText(img_, ss.str(), cells_[i][j].middle_, cv::FONT_HERSHEY_DUPLEX,
                        1.0, text_color_);
        }
    }
}

void HungarianLogger::cover_rows_and_cols(const std::vector<bool> &covered_rows, const std::vector<bool> &covered_cols)
{
    assert(covered_rows.size() == n_rows_ && covered_cols.size() == n_rows_);

    cv::Point row_shift(0, 0.5 * step_size_);
    cv::Point col_shift(0.5 * step_size_, 0);
    for (int k = 0; k < n_rows_; k++)
    {
        if (covered_rows[k])
        {
            auto &row = grid_rows_[k];
            cv::line(img_, row.first + row_shift, row.second + row_shift, line_color_);
        }
        if (covered_cols[k])
        {
            auto &col = grid_cols_[k];
            cv::line(img_, col.first + col_shift, col.second + col_shift, line_color_);
        }
    }
}

void HungarianLogger::draw_zeros(const std::map<int, int> &selected_in_row, const std::map<int, int> &prepared_in_row)
{
    for (const auto &ij : selected_in_row)
    {
        auto &cell = cells_[ij.first][ij.second];
        cv::rectangle(img_, cell.top_left_, cell.bottom_right_, selected_zero_color_);
    }
    for (const auto &ij : prepared_in_row)
    {
        auto &cell = cells_[ij.first][ij.second];
        cv::rectangle(img_, cell.top_left_, cell.bottom_right_, prepared_zero_color_);
    }
}

void HungarianLogger::draw_alternated_zeros_serie(const std::vector<std::pair<int, int>> &prepared_serie,
                                                  const std::vector<std::pair<int, int>> &selected_serie)
{
    auto it_prep = prepared_serie.cbegin();
    auto it_prep_next = prepared_serie.cbegin();
    it_prep_next++;

    for (auto it_select = selected_serie.cbegin(); it_select != selected_serie.cend(); it_select++, it_prep++, it_prep_next++)
    {
        cv::Point select_pt = cells_[it_select->first][it_select->second].middle_;
        cv::Point prep_pt = cells_[it_prep->first][it_prep->second].middle_;
        cv::line(img_, prep_pt, select_pt, alternated_serie_color_);
        if (it_prep_next != prepared_serie.cend())
        {
            cv::Point prep_next_pt = cells_[it_prep_next->first][it_prep_next->second].middle_;
            cv::line(img_, select_pt, prep_next_pt, alternated_serie_color_);
        }
    }
}

void HungarianLogger::imwrite()
{
    std::stringstream ss;
    ss << output_filename_base_ << current_step_ << ".png";
    cv::imwrite(ss.str(), img_);

    current_step_++; // Increment the current step
}