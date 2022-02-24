#include <iostream>
#include <IRA>
#include "SRTMinorServoCommandLibrary.h"

using namespace IRA;

int main(void)
{
    // Skipping std::endl since the protocol adds a \n already
    std::cout << SRTMinorServoCommandLibrary::status();
    std::cout << SRTMinorServoCommandLibrary::status("SRP");
    std::cout << SRTMinorServoCommandLibrary::setup("CCB");
    std::cout << SRTMinorServoCommandLibrary::stow("PFP");
    std::cout << SRTMinorServoCommandLibrary::stow("PFP", 1);
    std::cout << SRTMinorServoCommandLibrary::stop("PFP");
    std::cout << SRTMinorServoCommandLibrary::preset("PFP", std::vector<double>{0.,1.,2.,3.,4.,5.});

    double start_time = CIRATools::getUNIXEpoch() + 3;
    long unsigned int trajectory_id = (long unsigned int)(start_time * 1000);
    std::cout << SRTMinorServoCommandLibrary::programTrack("PFP", trajectory_id, 0, std::vector<double>{0.,1.,2.,3.,4.,5.}, start_time);
    for(unsigned int i = 1; i < 10; i++)
    {
        std::cout << SRTMinorServoCommandLibrary::programTrack("PFP", trajectory_id, i, std::vector<double>{0.,1.,2.,3.,4.,5.});
    }

    std::cout << SRTMinorServoCommandLibrary::offset("PFP", std::vector<double>{0.,1.,2.,3.,4.,5.});
}
