#ifndef ACTIVESURFACEPROXY_H
#define ACTIVESURFACEPROXY_H

#include <ActiveSurfaceBossC.h>
#include <lanC.h>
#include <usdC.h>

#include "ComponentProxy.h"

namespace ActiveSurface
{
    PROXY_COMPONENT(ActiveSurfaceBoss);
    PROXY_COMPONENT(lan);
    PROXY_COMPONENT(USD);
};

#endif
