#ifndef BACKENDSPROXY_H
#define BACKENDSPROXY_H

#include <BackendsBossC.h>
#include <DBBCC.h>
#include <HolographyC.h>
#include <NoiseGeneratorC.h>
#include <TotalPowerC.h>

#include "ComponentProxy.h"

namespace Backends
{
    PROXY_COMPONENT(BackendsBoss);
    PROXY_COMPONENT(DBBC);
    PROXY_COMPONENT(Holography);
    PROXY_COMPONENT(NoiseGenerator);
    PROXY_COMPONENT(TotalPower);
};

#endif
