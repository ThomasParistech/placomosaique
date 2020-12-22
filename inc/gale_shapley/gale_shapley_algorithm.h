/*********************************************************************************************************************
 * File : gale_shapley_algorithm.h                                                                                   *
 *                                                                                                                   *
 * 2020 Thomas Rouch                                                                                                 *
 *********************************************************************************************************************/

#ifndef GALE_SHAPLEY_ALGORITHM_H
#define GALE_SHAPLEY_ALGORITHM_H

#include <vector>

#include "gale_shapley/gale_shapley_man.h"
#include "gale_shapley/gale_shapley_woman.h"

/// @brief Algorithm for finding a solution to a stable matching problem, when there are more men than women and when
/// affinity scores are reciprocal, i.e. a man likes a woman as much as she likes him.
///
/// Obviously, there will remain single men.
class GaleShapleyAlgorithm
{
public:
    /// @brief Default constructor
    GaleShapleyAlgorithm();

    /// @brief Loads input love scores, solves the stable matching problem and return the optimal matches
    /// @param input_scores Coefficient [i][j] corresponds to the love score between a man i and a woman j. The lower
    /// the score the better
    /// @param output_matches Coefficient [i] corresponds to the index of the man engaged to the woman i
    /// @return true if the problem has been succesfully solved
    bool solve(const std::vector<std::vector<double>> &input_scores, std::vector<size_t> &output_matches);

private:
    /// @brief Finds a solution to the stable matching problem
    /// @return true if the problem has been succesfully solved
    bool find_stable_configuration();

    std::vector<Man> men_;
    std::vector<Woman> women_;
    std::vector<std::vector<double>> scores_;
};

#endif // GALE_SHAPLEY_ALGORITHM_H