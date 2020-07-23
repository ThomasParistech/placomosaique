/*********************************************************************************************************************
 * File : capsules_solver_app.cpp                                                                                    *
 *                                                                                                                   *
 * 2020 Thomas Rouch                                                                                                 *
 *********************************************************************************************************************/

#include <capsules_solver.h>

int main(int argc, char **argv)
{
    const cv::Mat img = cv::imread("/home/trouch/Images/hermanville.jpg");
    CapsulesSolver solver;
    solver.solve(img, "/tmp/Capsules", 20);
}
