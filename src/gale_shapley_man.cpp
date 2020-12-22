/*********************************************************************************************************************
 * File : gale_shapley_man.cpp                                                                                       *
 *                                                                                                                   *
 * 2020 Thomas Rouch                                                                                                 *
 *********************************************************************************************************************/

#include "gale_shapley/gale_shapley_man.h"

Man::Man(const std::vector<int> &sorted_women_indices) : engaged_(false)
{
    for (auto it = sorted_women_indices.cbegin(); it != sorted_women_indices.cend(); it++)
        sorted_women_.push(*it);
}

bool Man::propose_to_best_woman(size_t &best_woman_id)
{
    if (sorted_women_.empty())
        return false;

    best_woman_id = sorted_women_.top();
    sorted_women_.pop();
    return true;
}

void Man::engage()
{
    engaged_ = true;
}

void Man::break_engagement()
{
    engaged_ = false;
}

bool Man::is_engaged() const
{
    return engaged_;
}
