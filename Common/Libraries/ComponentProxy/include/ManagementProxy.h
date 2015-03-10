#ifndef MANAGEMENTPROXY_H
#define MANAGEMENTPROXY_H

#include <ExternalClientsC.h>
#include <FitsWriterC.h>
#include <CalibrationToolC.h>
#include <CustomLoggerC.h>
#include <SchedulerC.h>

#include "ComponentProxy.h"

namespace Management
{
    PROXY_COMPONENT(CustomLogger);
    PROXY_COMPONENT(ExternalClients);
    PROXY_COMPONENT(Scheduler);
    PROXY_COMPONENT(FitsWriter);
    PROXY_COMPONENT(CalibrationTool);
};

#endif

