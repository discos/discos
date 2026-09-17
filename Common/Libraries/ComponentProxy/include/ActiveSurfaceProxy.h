#ifndef ACTIVESURFACEPROXY_H
#define ACTIVESURFACEPROXY_H

#include <ActiveSurfaceBossC.h>
#include <ActiveSurfaceUSDC.h>
#include <ActiveSurfaceLANC.h>
#include <ActiveSurfaceSectorC.h>

#include "ComponentProxy.h"

namespace ActiveSurface
{
    PROXY_COMPONENT(ActiveSurfaceBoss);
    PROXY_COMPONENT(Sector)
    PROXY_COMPONENT(LAN);
    PROXY_COMPONENT(USD);
};

#endif
