#ifndef MINORSERVOPROXY_H
#define MINORSERVOPROXY_H

#include <MinorServoBossC.h>
#include <MinorServoC.h>

#include "ComponentProxy.h"

namespace MinorServo
{
    PROXY_COMPONENT(MinorServoBoss);
    PROXY_COMPONENT(WPServo);
};

#endif
