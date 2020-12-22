/*********************************************************************************************************************
 * File : gale_shapley_man.h                                                                                         *
 *                                                                                                                   *
 * 2020 Thomas Rouch                                                                                                 *
 *********************************************************************************************************************/

#ifndef GALE_SHAPLEY_MAN_H
#define GALE_SHAPLEY_MAN_H

#include <stack>
#include <vector>

/// @brief Class representing a man in the Gale-Shapley Algorithm
class Man
{
public:
    /// @brief Constructor
    /// @param sorted_women_indices List of the women indices sorted according to its preferences (Best women on top)
    Man(const std::vector<int> &sorted_women_indices);

    /// @brief Proposes to the woman he likes the most of those he has not yet proposed to
    /// @param best_woman_id Output id of the woman he wants to proposed to
    /// @return false if he has already proposed to all the women
    bool propose_to_best_woman(size_t &best_woman_id);

    /// @brief Sets him as engaged
    void engage();

    /// @brief Sets him as unengaged
    void break_engagement();

    /// @brief Checks if he's engaged
    bool is_engaged() const;

private:
    bool engaged_;
    std::stack<size_t> sorted_women_; ///< Best women on top
};

#endif // GALE_SHAPLEY_MAN_H