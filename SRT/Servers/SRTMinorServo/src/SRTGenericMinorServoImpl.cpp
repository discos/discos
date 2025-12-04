#include "SRTMinorServoImpl.h"

SRTGenericMinorServoImpl::SRTGenericMinorServoImpl(const ACE_CString &componentName, maci::ContainerServices *containerServices) : SRTBaseMinorServoImpl(componentName, containerServices)
{
    AUTO_TRACE(m_servo_name + "::SRTGenericMinorServoImpl()");
}

SRTGenericMinorServoImpl::~SRTGenericMinorServoImpl()
{
    AUTO_TRACE(m_servo_name + "::~SRTGenericMinorServoImpl()");
}

MACI_DLL_SUPPORT_FUNCTIONS(SRTGenericMinorServoImpl)
