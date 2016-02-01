#ifndef RECEIVERSPROXY_H
#define RECEIVERSPROXY_H

#include <ReceiversBossC.h>
#include <DewarPositionerC.h>
#include <LocalOscillatorInterfaceC.h>

#include "ComponentProxy.h"

namespace Receivers
{
    PROXY_COMPONENT(ReceiversBoss);
    PROXY_COMPONENT(DewarPositioner);
    PROXY_COMPONENT(LocalOscillator);
};

#endif

