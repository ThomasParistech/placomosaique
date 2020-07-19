/*********************************************************************************************************************
 * File : gale_shapley_utils.h                                                                                       *
 *                                                                                                                   *
 * 2020 Thomas Rouch                                                                                                 *
 *********************************************************************************************************************/

#ifndef GALE_SHAPLEY_UTILS_H
#define GALE_SHAPLEY_UTILS_H

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
    bool engaged;
    std::stack<size_t> sorted_women; ///< Best women on top
};

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

#endif // GALE_SHAPLEY_UTILS_H