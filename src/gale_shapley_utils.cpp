/*********************************************************************************************************************
 * File : gale_shapley_algorithm.cpp                                                                                 *
 *                                                                                                                   *
 * 2020 Thomas Rouch                                                                                                 *
 *********************************************************************************************************************/

#include <algorithm>
#include <iostream>

#include "gale_shapley_utils.h"

size_t Woman::number_of_engaged_women = 0;

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

Woman::Woman(const std::vector<double> &men_scores) : engaged_man_id(-1), engaged_score(-1), men_scores(men_scores) {}

void Woman::add_proposal(size_t man_id)
{
    proposals.push_back(man_id);
}

size_t Woman::get_man_id() const
{
    return engaged_man_id;
}

bool Woman::update_engagement(size_t &old_man_id, size_t &new_man_id)
{
    if (proposals.empty())
        return false;

    auto it_best = std::min_element(proposals.cbegin(), proposals.cend(),
                                    [&](const size_t a, const size_t b) { return men_scores[a] < men_scores[b]; });
    const size_t best_man_id = *it_best;
    const double best_man_score = men_scores[best_man_id];

    proposals.clear();

    if (engaged_man_id == -1)
        number_of_engaged_women++;
    else if (best_man_score >= engaged_score)
        return false;

    old_man_id = engaged_man_id;
    new_man_id = best_man_id;

    engaged_man_id = best_man_id;
    engaged_score = best_man_score;
    return true;
}
