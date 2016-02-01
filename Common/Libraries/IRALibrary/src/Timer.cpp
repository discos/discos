#include "Timer.h"

using namespace IRA;

CTimer::CTimer()
{
    _start_time = CIRATools::getACSTime();
}

ACS::Time
CTimer::elapsed()
{
    return  CIRATools::getACSTime() - _start_time;
}

void
CTimer::reset()
{
    _start_time = CIRATools::getACSTime();
}

