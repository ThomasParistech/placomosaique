/*********************************************************************************************************************
 * File : hungarian_method.h                                                                                         *
 *                                                                                                                   *
 *                                                                                                                   *
 *                                                                                                                   *
 *********************************************************************************************************************/

#ifndef HUNGARIAN_METHOD_H
#define HUNGARIAN_METHOD_H

#include <map>
#include <vector>
#include <memory>
#include <opencv2/opencv.hpp>
#include "hungarian_logger.h"

/// @brief
class HungarianMethod
{
public:
    HungarianMethod(const std::vector<std::vector<double>> &grid, bool save_images = true);

    inline const std::vector<std::vector<double>> &get_grid() const
    {
        return grid_;
    }

    void solve();

private:
    void select_independent_zeros();
    void remove_smallest_uncovered_value();
    bool find_uncovered_zero(int &i_zero, int &j_zero);
    void build_alternated_series_of_zeros(int i, int j);

    bool is_optimal();

    ////

    void prepared_to_selected(int j);
    void selected_to_prepared(int i);

    int size_;
    std::vector<std::vector<double>> grid_;

    /// Selected Zeros
    std::map<int, int> selected_in_row_;        ///< Row -> Col
    std::map<int, int> selected_in_col_;        ///< Col -> Row
    std::vector<bool> has_row_a_selected_zero_; ///< Rows with selected zero
    std::vector<bool> has_col_a_selected_zero_; ///< Cols with selected zero

    /// Prepared Zeros
    std::map<int, int> prepared_in_row_; ///< Row -> Col

    /// Alternated serie
    std::vector<std::pair<int, int>> prepared_serie_;
    std::vector<std::pair<int, int>> selected_serie_;

    /// Covered rows and cols
    std::vector<bool> covered_rows_;
    std::vector<bool> covered_cols_;
    std::vector<double *> cells_to_increment_;
    std::vector<double *> cells_to_decrement_;

    /// Display
    std::unique_ptr<HungarianLogger> logger_;
};

/// @brief convert a HungarianMethod instance to a string to append to a std::ostream
inline std::ostream &operator<<(std::ostream &ostr, const HungarianMethod &hm)
{
    auto &grid = hm.get_grid();
    for (auto row = grid.begin(); row != grid.end(); row++)
    {
        for (auto val = row->begin(); val != row->end(); val++)
            ostr << *val << ", ";
        ostr << std::endl;
    }
    return ostr;
}

#endif // HUNGARIAN_METHOD_H
