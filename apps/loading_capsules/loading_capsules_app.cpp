/*********************************************************************************************************************
 * File : loading_capsules_app.cpp                                                                                   *
 *                                                                                                                   *
 * 2020 Thomas Rouch                                                                                                 *
 *********************************************************************************************************************/

#include <timer.h>
#include <capsule_extractor.h>

int main(int argc, char **argv)
{
    CapsuleExtractionPattern capsule_pattern(2160, 1630, 58, 20, 6, 5, 140);
    CapsuleExtractor extractor(capsule_pattern);
    {
        Timer timer("Extract and save capsules", Timer::MS);
        extractor.extract_capsules_from_directory("/home/trouch/Images/capsules/");
    }
}
