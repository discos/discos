#include "SRTMinorServoImpl.h"

using namespace maci;

SRTProgramTrackMinorServoImpl::SRTProgramTrackMinorServoImpl(const ACE_CString &componentName, maci::ContainerServices *containerServices) : SRTBaseMinorServoImpl(componentName, containerServices)
{
    AUTO_TRACE(m_servo_name + "::SRTProgramTrackMinorServoImpl()");
}

SRTProgramTrackMinorServoImpl::~SRTProgramTrackMinorServoImpl()
{
    AUTO_TRACE(m_servo_name + "::~SRTProgramTrackMinorServoImpl()");
}

void SRTProgramTrackMinorServoImpl::programTrack(CORBA::Long trajectory_id, CORBA::Long point_id, CORBA::Double start_time, const ACS::doubleSeq& virtual_coordinates)
{
    AUTO_TRACE("SRTProgramTrackMinorServoImpl::programTrack()");

    if(virtual_coordinates.length() != m_virtual_axes)
    {
        //Wrong number of virtual_coordinates
    }

    std::vector<double> coords(virtual_coordinates.get_buffer(), virtual_coordinates.get_buffer() + virtual_coordinates.length());

    SRTMinorServoAnswerMap answer = m_socket->sendCommand(SRTMinorServoCommandLibrary::programTrack(m_servo_name, trajectory_id, point_id, coords, start_time));

    if(std::get<std::string>(answer["OUTPUT"]) != "GOOD")
    {
        std::cout << "Cannot execute programTrack" << std::endl;
    }
}

MACI_DLL_SUPPORT_FUNCTIONS(SRTProgramTrackMinorServoImpl)
