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
        _EXCPT(MinorServoErrors::TrackingErrorExImpl, impl, (m_servo_name + "::programTrack()").c_str());
        impl.addData("Reason", "Wrong number of values for this servo system!");
        throw impl;
    }

    std::vector<double> coordinates(virtual_coordinates.get_buffer(), virtual_coordinates.get_buffer() + virtual_coordinates.length());

    SRTMinorServoAnswerMap answer = m_socket->sendCommand(SRTMinorServoCommandLibrary::programTrack(m_servo_name, trajectory_id, point_id, coordinates, start_time));

    if(std::get<std::string>(answer["OUTPUT"]) != "GOOD")
    {
        _EXCPT(MinorServoErrors::CommunicationErrorExImpl, impl, (m_servo_name + "::programTrack()").c_str());
        impl.setReason("Received NAK in response to a PRESET command!");
        throw impl;
    }
}

MACI_DLL_SUPPORT_FUNCTIONS(SRTProgramTrackMinorServoImpl)
