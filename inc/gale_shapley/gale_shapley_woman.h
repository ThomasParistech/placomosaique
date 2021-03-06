/*********************************************************************************************************************
 * File : gale_shapley_woman.h                                                                                       *
 *                                                                                                                   *
 * 2020 Thomas Rouch                                                                                                 *
 *********************************************************************************************************************/

#ifndef GALE_SHAPLEY_WOMAN_H
#define GALE_SHAPLEY_WOMAN_H

#include <vector>

/// @brief Class representing a woman in the Gale-Shapley Algorithm
class Woman
{
public:
    /// @brief Constructor
    /// @param men_scores Coefficient [i] corresponds to the love score with the man i. The lower the score the better
    Woman(const std::vector<double> &men_scores);

    /// @brief Stores a new proposal
    /// @param man_id Id of the man proposing
    void add_proposal(size_t man_id);

    /// @brief Gets the id of the man shes's currently engaged to
    size_t get_man_id() const;

    /// @brief Looks over the proposals, finds the best man and accepts it if he's better than the man
    /// she's already engaged to
    /// @param old_man_id Id of the previous engaged man
    /// @param new_man_id Id of the new engaged man
    /// @return true if she's decided to get engaged with a new man
    bool update_engagement(size_t &old_man_id, size_t &new_man_id);

    static size_t number_of_engaged_women;

private:
    int engaged_man_id;
    double engaged_score;
    std::vector<size_t> proposals;
    std::vector<double> men_scores; ///< Scores, used to convert man indices to actual love scores
};

#endif // GALE_SHAPLEY_WOMAN_H