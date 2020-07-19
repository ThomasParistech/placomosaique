/*********************************************************************************************************************
 * File : gale_shapley_algorithm.cpp                                                                                 *
 *                                                                                                                   *
 * 2020 Thomas Rouch                                                                                                 *
 *********************************************************************************************************************/

#include <algorithm>
#include <iostream>
#include <numeric>

#include "gale_shapley_algorithm.h"

GaleShapleyAlgorithm::GaleShapleyAlgorithm() {}

bool GaleShapleyAlgorithm::solve(const std::vector<std::vector<double>> &input_scores, std::vector<size_t> &output_matches)
{
    // Check if there are enough men
    const int n_men = input_scores.size();
    const int n_women = input_scores[0].size();
    if (n_men < n_women)
    {
        std::cerr << "There's not enough men to get each woman engaged. Got " << n_men << " men and "
                  << n_women << " women." << std::endl;
        return false;
    }

    // Men
    std::vector<int> women_indices(n_women);
    std::iota(women_indices.begin(), women_indices.end(), 0); // List of women indices
    men_.reserve(n_men);
    for (const auto &women_scores : input_scores)
    {
        if (women_scores.size() != n_women)
        {
            std::cerr << "Wrong array format. Each row must have the same size. The first rows has "
                      << n_women << " scores." << std::endl;
            return false;
        }
        std::sort(women_indices.begin(), women_indices.end(),
                  [&women_scores](int a, int b) { return women_scores[a] > women_scores[b]; });
        // Initialize a man with a list of the women indices sorted according to its preferences
        men_.emplace_back(women_indices);
    }

    // Women
    women_.reserve(n_women);
    std::vector<double> men_scores(n_men, -1);
    for (size_t j = 0; j < n_women; j++)
    {
        for (size_t i = 0; i < n_men; i++)
            men_scores[i] = input_scores[i][j];
        women_.emplace_back(men_scores);
    }

    std::cout << "Gale-Shapley Algorithm: " << men_.size() << " men and " << women_.size() << " women." << std::endl;

    // Solve
    if (!find_stable_configuration())
        return false;

    output_matches.clear();
    output_matches.reserve(women_.size());
    for (const auto &woman : women_)
        output_matches.emplace_back(woman.get_man_id());

    return true;
}

bool GaleShapleyAlgorithm::find_stable_configuration()
{
    std::cout << "Starts solving..." << std::endl;
    bool men_keep_proposing = true; // More optimal to use this as termination criterion !
    int last_decile = 0;            // From 0 to 10
    while (men_keep_proposing)
    {
        men_keep_proposing = false;
        int n_changes = 0;
        // Men propose
        size_t man_id = 0;
        for (auto it_man = men_.begin(); it_man != men_.end(); it_man++, man_id++)
        {
            if (it_man->is_engaged())
                continue;

            size_t best_woman_id;
            if (!it_man->propose_to_best_woman(best_woman_id))
                continue;

            men_keep_proposing = true;
            women_[best_woman_id].add_proposal(man_id);
        }

        // Women dispose
        for (auto &woman : women_)
        {
            size_t old_man_id, new_man_id;
            if (woman.update_engagement(old_man_id, new_man_id))
            {
                n_changes++;
                men_[old_man_id].break_engagement();
                men_[new_man_id].engage();
            }
        }

        const int decile = (10 * Woman::number_of_engaged_women) / women_.size();
        if (decile > last_decile)
        {
            last_decile = decile;
            std::cout << "Engaged women: " << 10 * decile << "%" << std::endl;
        }
    }

    return true;
}
