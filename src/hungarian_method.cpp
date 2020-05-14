/*********************************************************************************************************************
 * File : hungarian_method.cpp                                                                                       *
 *                                                                                                                   *
 *                                                                                                                   *
 *                                                                                                                   *
 *********************************************************************************************************************/

#include <iostream>
#include <stdexcept>
#include <algorithm>

#include "hungarian_method.h"

HungarianMethod::HungarianMethod(const std::vector<std::vector<double>> &vec, bool save_images)
{
    /// 1) Make sure the grid is a square
    size_ = vec.size();
    for (auto it = vec.begin(); it != vec.end(); it++)
        if (it->size() != size_)
            throw std::invalid_argument("Invalid grid size");
    grid_ = vec;

    if (save_images)
    {
        logger_.reset(new HungarianLogger(size_));
        logger_->draw_numbers(grid_);
        logger_->imwrite();
    }

    /// 2) Subtract min from rows
    for (auto it = grid_.begin(); it != grid_.end(); ++it)
    {
        auto row_min = *std::min_element(it->begin(), it->end());
        std::for_each(it->begin(), it->end(), [row_min](double &val) { val -= row_min; });
    }

    /// 3) Subtract min from cols
    std::vector<double *> col_ptrs;
    col_ptrs.reserve(size_);
    for (size_t j = 0; j < size_; j++)
    {
        // Stores pointers to the column
        col_ptrs.clear();
        for (size_t i = 0; i < size_; i++)
            col_ptrs.push_back(&grid_[i][j]);

        // Subtract min from col
        auto col_min = *(*std::min_element(col_ptrs.begin(), col_ptrs.end(),
                                           [](const double *l, const double *r) { return (*l < *r); }));
        std::for_each(col_ptrs.begin(), col_ptrs.end(), [col_min](double *val) { *val -= col_min; });
    }

    if (logger_)
    {
        logger_->draw_numbers(grid_);
        logger_->imwrite();
    }
}

bool HungarianMethod::is_optimal()
{
    const int n_selected_zeros = std::count(covered_cols_.begin(), covered_cols_.end(), true);
    // std::cout << "Selected zeros : " << n_selected_zeros << "/" << size_ << std::endl;
    return n_selected_zeros == size_;
}

void HungarianMethod::solve()
{
    select_independent_zeros();
    covered_cols_ = has_col_a_selected_zero_; /// Cover columns with a selected zero
    covered_rows_.clear();
    covered_rows_.resize(size_, false); /// Uncover the rows

    if (logger_)
    {
        logger_->draw_numbers(grid_);
        logger_->cover_rows_and_cols(covered_rows_, covered_cols_);
        logger_->draw_zeros(selected_in_row_, prepared_in_row_);
        logger_->imwrite();
    }

    std::cout << ">>> Start solving the assignment problem" << std::endl;
    int i, j, j_selected;
    while (!is_optimal())
    {
        /// Try and find an uncovered zero Z0, and set it "prepared"
        if (!find_uncovered_zero(i, j))
        {
            remove_smallest_uncovered_value();
            continue;
        }

        /// Look for a selected zero Z1 on the row of Z0
        if (has_row_a_selected_zero_[i])
        {
            /// Cover row and uncover column corresponding to the selected zero Z1
            j_selected = selected_in_row_[i];
            covered_rows_[i] = true;
            covered_cols_[j_selected] = false;
            remove_smallest_uncovered_value();
        }
        else
            build_alternated_series_of_zeros(i, j);

        if (logger_)
        {
            logger_->draw_numbers(grid_);
            logger_->cover_rows_and_cols(covered_rows_, covered_cols_);
            logger_->draw_zeros(selected_in_row_, prepared_in_row_);
            logger_->imwrite();
        }
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void HungarianMethod::remove_smallest_uncovered_value()
{
    /// If the cell
    ///  - isn't covered at all           : add it to cells_to_decrement_
    ///  - is twice-covered (row AND col) : add it to cells_to_increment_
    cells_to_increment_.clear();
    cells_to_decrement_.clear();
    cells_to_increment_.reserve(size_);
    cells_to_decrement_.reserve(size_ * size_);
    for (size_t i = 0; i < size_; i++)
    {
        // covered row
        const bool is_row_crossed = covered_rows_[i];
        auto &row_i = grid_[i];
        for (size_t j = 0; j < size_; j++)
        {
            // covered col
            const bool is_col_crossed = covered_cols_[j];
            if (is_row_crossed && is_col_crossed)
                cells_to_increment_.push_back(&row_i[j]);
            else if (!is_row_crossed && !is_col_crossed)
                cells_to_decrement_.push_back(&row_i[j]);
        }
    }

    /// Find smallest non covered value and :
    ///  - subtract it to the cells in cells_to_decrement_
    ///  -      add it to the cells in cells_to_increment_
    auto min_val = *(*std::min_element(cells_to_decrement_.cbegin(), cells_to_decrement_.cend(),
                                       [](const double *l, const double *r) { return (*l < *r); }));
    std::for_each(cells_to_decrement_.begin(), cells_to_decrement_.end(), [min_val](double *c) { *c -= min_val; });
    std::for_each(cells_to_increment_.begin(), cells_to_increment_.end(), [min_val](double *c) { *c += min_val; });
}

bool HungarianMethod::find_uncovered_zero(int &i_zero, int &j_zero)
{
    int i = 0;
    auto it_row = grid_.begin();
    for (auto it_cov_row = covered_rows_.begin();
         it_cov_row != covered_rows_.end();
         it_cov_row++, it_row++, i++)
    {
        if (!*it_cov_row)
        {
            int j = 0;
            auto it_val = it_row->begin();
            for (auto it_cov_col = covered_cols_.begin();
                 it_cov_col != covered_cols_.end();
                 it_cov_col++, it_val++, j++)
            {
                if (!*it_cov_col && *it_val == 0)
                {
                    /// Add prepared zero
                    i_zero = i;
                    j_zero = j;
                    prepared_in_row_[i_zero] = j_zero;
                    return true;
                }
            }
        }
    }
    return false;
}

void HungarianMethod::select_independent_zeros()
{
    /// Count zeros on rows
    std::map<int, int> zero_counts_on_rows;
    int i = 0;
    for (auto it_row = grid_.cbegin(); it_row != grid_.cend(); it_row++, i++)
        zero_counts_on_rows[i] = std::count(it_row->cbegin(), it_row->cend(), 0);

    /// Reset info for selected zeros
    selected_in_row_.clear();
    selected_in_col_.clear();
    has_row_a_selected_zero_.clear();
    has_row_a_selected_zero_.resize(size_, false);
    has_col_a_selected_zero_.clear();
    has_col_a_selected_zero_.resize(size_, false);

    /// Select zeros
    while (!zero_counts_on_rows.empty())
    {
        /// 1) Get row with less zeros
        auto it_min = std::min_element(zero_counts_on_rows.begin(), zero_counts_on_rows.end(),
                                       [](const std::pair<int, int> &left,
                                          const std::pair<int, int> &right) { return (left.second < right.second); });
        const int i = it_min->first;
        std::cout << "Select 0 in row " << i << std::endl;
        assert(it_min->second != 0);

        /// 2) Select first zero on row
        auto &row_i = grid_[i];
        int j = 0;
        auto it_check = has_col_a_selected_zero_.begin();
        for (auto it_val = row_i.begin(); it_val != row_i.end(); it_val++, it_check++, j++)
        {
            if (*it_val == 0 && !*it_check)
            {
                // Select zero
                selected_in_row_[i] = j;
                has_row_a_selected_zero_[i] = true;
                selected_in_col_[j] = i;
                has_col_a_selected_zero_[j] = true;

                // Remove current row from counts
                zero_counts_on_rows.erase(i);

                // Look for other zeros on column j (different than row i)
                // and decrement the count on their rows
                auto it = zero_counts_on_rows.begin();
                while (it != zero_counts_on_rows.end())
                {
                    if (grid_[it->first][j] == 0)
                    {
                        it->second--;
                        /// Remove the row if there's no zero left
                        if (it->second == 0)
                        {
                            it = zero_counts_on_rows.erase(it);
                            continue;
                        }
                    }
                    it++;
                }

                /// Get out of the loop since we've found the first zero
                break;
            }
        }
    }
}

void HungarianMethod::build_alternated_series_of_zeros(int i, int j)
{
    prepared_serie_.clear();
    selected_serie_.clear();

    /// 1) Start from "prepared" zero at (i,j) and build alternated serie
    prepared_serie_.emplace_back(std::make_pair(i, j));
    prepared_to_selected(j);

    if (logger_)
    {
        logger_->draw_numbers(grid_);
        logger_->cover_rows_and_cols(covered_rows_, covered_cols_);
        logger_->draw_zeros(selected_in_row_, prepared_in_row_);
        logger_->draw_alternated_zeros_serie(prepared_serie_, selected_serie_);
        logger_->imwrite();
    }

    /// 2) Remove the selected zeros of the serie
    for (const auto &ij : selected_serie_)
    {
        const int i = ij.first;
        selected_in_row_.erase(i);
        has_row_a_selected_zero_[i] = false;

        const int j = ij.second;
        selected_in_col_.erase(j);
        has_col_a_selected_zero_[j] = false;
    }

    /// 3) Select the "prepared" zeros of the serie
    for (const auto &ij : prepared_serie_)
    {
        const int i = ij.first;
        const int j = ij.second;

        selected_in_row_[i] = j;
        has_row_a_selected_zero_[i] = true;

        selected_in_col_[j] = i;
        has_col_a_selected_zero_[j] = true;
    }

    /// 4) Remove all "prepared" zeros
    prepared_in_row_.clear();

    /// 5) Uncover rows and cover columns with selected zeros
    covered_cols_ = has_col_a_selected_zero_;
    std::fill(covered_rows_.begin(), covered_rows_.end(), false);
}

void HungarianMethod::prepared_to_selected(int j)
{
    /// Look for a selected zero inside the column j
    if (!has_col_a_selected_zero_[j])
        return;

    const int i = selected_in_col_[j];
    selected_serie_.emplace_back(std::make_pair(i, j));
    selected_to_prepared(i);
}

void HungarianMethod::selected_to_prepared(int i)
{
    /// Look for a prepared zero inside the row i
    auto it = prepared_in_row_.find(i);
    if (it == prepared_in_row_.end())
        return;

    const int j = it->second;
    prepared_serie_.emplace_back(std::make_pair(i, j));
    prepared_to_selected(j);
}
