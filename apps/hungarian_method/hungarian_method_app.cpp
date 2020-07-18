/*********************************************************************************************************************
 * File : hungarian_method_app.cpp                                                                                   *
 *                                                                                                                   *
 * 2020 Thomas Rouch                                                                                                 *
 *********************************************************************************************************************/

#include <random>
#include <iostream>
#include <algorithm>
#include <numeric>
#include <vector>

#include <hungarian_method.h>

int main(int argc, char **argv)
{
    enum Case
    {
        Random,
        SegFault,
        Fail
    };

    std::vector<std::vector<double>> grid;

    Case case_ = Random;
    switch (case_)
    {
    case Random:
    {
        int size = 6;
        std::random_device rd;
        std::mt19937 g(rd());

        for (size_t i = 0; i < size; i++)
        {
            grid.emplace_back();
            auto &back = grid.back();
            back.resize(size);
            std::iota(back.begin(), back.end(), 3);
            std::shuffle(back.begin(), back.end(), g);
        }
    }
    break;
    case SegFault:
    {
        grid.emplace_back(std::vector<double>{6, 4, 5, 3});
        grid.emplace_back(std::vector<double>{6, 4, 3, 5});
        grid.emplace_back(std::vector<double>{6, 4, 5, 3});
        grid.emplace_back(std::vector<double>{6, 4, 5, 3});
    }
    break;
    case Fail:
    {
        grid.emplace_back(std::vector<double>{5, 4, 6, 3});
        grid.emplace_back(std::vector<double>{3, 4, 6, 5});
        grid.emplace_back(std::vector<double>{3, 4, 5, 6});
        grid.emplace_back(std::vector<double>{5, 4, 3, 6});
    }
    }

    // std::cout << "Input Matrix" << std::endl;
    // for (auto it_row = grid.begin(); it_row != grid.end(); it_row++)
    // {
    //     for (auto it_val = it_row->begin(); it_val != it_row->end(); it_val++)
    //         std::cout << *it_val << " , ";
    //     std::cout << std::endl;
    // }
    // std::cout << std::endl;

    /////////////////

    HungarianMethod hm(grid);
    hm.solve();
    return 0;
}
