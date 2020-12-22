/*********************************************************************************************************************
 * File : gale_shapley_woman.cpp                                                                                     *
 *                                                                                                                   *
 * 2020 Thomas Rouch                                                                                                 *
 *********************************************************************************************************************/

#include <algorithm>

#include "gale_shapley/gale_shapley_woman.h"

size_t Woman::number_of_engaged_women = 0;

Woman::Woman(const std::vector<double> &men_scores) : engaged_man_id_(-1), engaged_score_(-1), men_scores_(men_scores) {}

void Woman::add_proposal(size_t man_id)
{
    proposals_.push_back(man_id);
}

size_t Woman::get_man_id() const
{
    return engaged_man_id_;
}

bool Woman::update_engagement(size_t &old_man_id, size_t &new_man_id)
{
    if (proposals_.empty())
        return false;

    auto it_best = std::min_element(proposals_.cbegin(), proposals_.cend(),
                                    [&](const size_t a, const size_t b) { return men_scores_[a] < men_scores_[b]; });
    const size_t best_man_id = *it_best;
    const double best_man_score = men_scores_[best_man_id];

    proposals_.clear();

    if (engaged_man_id_ == -1)
        number_of_engaged_women++;
    else if (best_man_score >= engaged_score_)
        return false;

    old_man_id = engaged_man_id_;
    new_man_id = best_man_id;

    engaged_man_id_ = best_man_id;
    engaged_score_ = best_man_score;
    return true;
}
