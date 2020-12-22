/*********************************************************************************************************************
 * File : gale_shapley_man.cpp                                                                                       *
 *                                                                                                                   *
 * 2020 Thomas Rouch                                                                                                 *
 *********************************************************************************************************************/

#include "gale_shapley/gale_shapley_man.h"

Man::Man(const std::vector<int> &sorted_women_indices) : engaged(false)
{
    for (auto it = sorted_women_indices.cbegin(); it != sorted_women_indices.cend(); it++)
        sorted_women.push(*it);
}

bool Man::propose_to_best_woman(size_t &best_woman_id)
{
    if (sorted_women.empty())
        return false;

    best_woman_id = sorted_women.top();
    sorted_women.pop();
    return true;
}

void Man::engage()
{
    engaged = true;
}

void Man::break_engagement()
{
    engaged = false;
}

bool Man::is_engaged() const
{
    return engaged;
}
