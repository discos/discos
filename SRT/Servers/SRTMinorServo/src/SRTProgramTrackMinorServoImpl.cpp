#include "SRTMinorServoImpl.h"

using namespace MinorServo;

SRTProgramTrackMinorServoImpl::SRTProgramTrackMinorServoImpl(const ACE_CString &componentName, maci::ContainerServices *containerServices) :
    SRTBaseMinorServoImpl(componentName, containerServices),
    m_tracking_queue(1500, m_virtual_axes),
    m_tracking_delta(SRTBaseMinorServoImpl::getMotionConstant(*this, "tracking_delta")),
    m_tracking(Management::MNG_FALSE),
    m_trajectory_id(0),
    m_total_trajectory_points(0),
    m_remaining_trajectory_points(0),
    m_tracking_ptr(this),
    m_trajectory_id_ptr(this),
    m_total_trajectory_points_ptr(this),
    m_remaining_trajectory_points_ptr(this)
{
    AUTO_TRACE(m_servo_name + "::SRTProgramTrackMinorServoImpl()");
}

SRTProgramTrackMinorServoImpl::~SRTProgramTrackMinorServoImpl()
{
    AUTO_TRACE(m_servo_name + "::~SRTProgramTrackMinorServoImpl()");
}

void SRTProgramTrackMinorServoImpl::initialize()
{
    SRTBaseMinorServoImpl::initialize();

    try
    {
        m_tracking_ptr = new ROEnumImpl<ACS_ENUM_T(Management::TBoolean), POA_Management::ROTBoolean>((m_component_name + ":tracking").c_str(), getComponent(),
                new MSGenericDevIO<Management::TBoolean, std::atomic<Management::TBoolean>>(m_tracking), true);
        m_trajectory_id_ptr = new baci::ROlong((m_component_name + ":trajectory_id").c_str(), getComponent(),
                new MSGenericDevIO<CORBA::Long, std::atomic<unsigned int>>(m_trajectory_id), true);
        m_total_trajectory_points_ptr = new baci::ROlong((m_component_name + ":total_trajectory_points").c_str(), getComponent(),
                new MSGenericDevIO<CORBA::Long, std::atomic<unsigned int>>(m_total_trajectory_points), true);
        m_remaining_trajectory_points_ptr = new baci::ROlong((m_component_name + ":remaining_trajectory_points").c_str(), getComponent(),
                new MSGenericDevIO<CORBA::Long, std::atomic<unsigned int>>(m_remaining_trajectory_points), true);
    }
    catch(std::bad_alloc& ba)
    {
        _EXCPT(ComponentErrors::MemoryAllocationExImpl, ex, (m_servo_name + "::initialize()").c_str());
        ex.log(LM_DEBUG);
        throw ex.getComponentErrorsEx();
    }
}

/////////////////// PUBLIC methods
bool SRTProgramTrackMinorServoImpl::status()
{
    bool status = SRTBaseMinorServoImpl::status();

    try
    {
        ACS::Time last_timestamp = m_status.getTimestamp();

        // The timestamp of the read positions always corresponds to the one we're asking since they both belong to the same STATUS command answer
        // The tracking timestamp is interpolated instead
        std::vector<double> virtual_positions = m_positions_queue.get(last_timestamp).second;
        std::pair<ACS::Time, std::vector<double>> tracking_point = m_tracking_queue.get(last_timestamp);

        m_remaining_trajectory_points.store(m_tracking_queue.getRemainingPoints(last_timestamp));

        if(tracking_point.first < last_timestamp)
        {
            // We are past the last point of the trajectory, we concluded it
            m_tracking.store(Management::MNG_FALSE);
            m_tracking_queue.clear();
        }
        else
        {
            bool is_tracking = true;

            for(size_t i = 0; i < virtual_positions.size(); i++)
            {
                if(std::fabs(virtual_positions[i] - tracking_point.second[i]) > m_tracking_delta[i])
                {
                    is_tracking = false;
                    break;
                }
            }

            m_tracking.store(is_tracking ? Management::MNG_TRUE : Management::MNG_FALSE);
        }
    }
    catch(...)
    {
        // We might get here if m_tracking_queue is empty
        // So whenever we just got a new setup or if we are past the last point inside the trajectory
        m_tracking.store(Management::MNG_FALSE);  // May be redundant but who cares?
    }

    return status;
}

bool SRTProgramTrackMinorServoImpl::setup(const char* configuration_name)
{
    bool return_value = SRTBaseMinorServoImpl::setup(configuration_name);

    m_tracking_queue.clear();
    m_total_trajectory_points.store(0);
    m_remaining_trajectory_points.store(0);

    return return_value;
}

void SRTProgramTrackMinorServoImpl::programTrack(CORBA::Long trajectory_id, CORBA::Long point_id, ACS::Time point_time, const ACS::doubleSeq& virtual_coordinates)
{
    AUTO_TRACE("SRTProgramTrackMinorServoImpl::programTrack()");

    checkLineStatus();

    if(virtual_coordinates.length() != m_virtual_axes)
    {
        _EXCPT(MinorServoErrors::TrackingErrorExImpl, ex, (m_servo_name + "::programTrack()").c_str());
        ex.addData("Reason", "Wrong number of values for this servo system!");
        ex.log(LM_DEBUG);
        throw ex.getMinorServoErrorsEx();
    }

    std::vector<double> coordinates(virtual_coordinates.get_buffer(), virtual_coordinates.get_buffer() + virtual_coordinates.length());
    ACS::Time comp;
    ACS::doubleSeq offsets = m_virtual_offsets_devio->read(comp);

    for(size_t i = 0; i < m_virtual_axes; i++)
    {
        double coordinate = coordinates[i] + offsets[i];
        if(coordinate < m_min[i] || coordinate > m_max[i])
        {
            _EXCPT(MinorServoErrors::TrackingErrorExImpl, ex, (m_servo_name + "::programTrack()").c_str());
            ex.addData("Reason", "Resulting position out of range, check the offsets!");
            ex.log(LM_DEBUG);
            throw ex.getMinorServoErrorsEx();
        }
    }

    if(!m_socket.sendCommand(SRTMinorServoCommandLibrary::programTrack(m_servo_name, trajectory_id, point_id, coordinates, point_id > 0 ? 0 : IRA::CIRATools::ACSTime2UNIXEpoch(point_time))).checkOutput())
    {
        _EXCPT(MinorServoErrors::CommunicationErrorExImpl, ex, (m_servo_name + "::programTrack()").c_str());
        ex.setReason("Received NAK in response to a PROGRAMTRACK command!");
        ex.log(LM_DEBUG);
        throw ex.getMinorServoErrorsEx();
    }

    m_trajectory_id.store(trajectory_id);
    m_total_trajectory_points.store(point_id + 1);
    if(point_id == 0)
    {
        // Clear the tracking queue to avoid interpolation between 2 different trajectories
        m_tracking_queue.clear();
    }
    m_tracking_queue.put(point_time, coordinates);
}


GET_PROPERTY_REFERENCE(Management::ROTBoolean, SRTProgramTrackMinorServoImpl, m_tracking_ptr, tracking);
GET_PROPERTY_REFERENCE(ACS::ROlong, SRTProgramTrackMinorServoImpl, m_trajectory_id_ptr, trajectory_id);
GET_PROPERTY_REFERENCE(ACS::ROlong, SRTProgramTrackMinorServoImpl, m_total_trajectory_points_ptr, total_trajectory_points);
GET_PROPERTY_REFERENCE(ACS::ROlong, SRTProgramTrackMinorServoImpl, m_remaining_trajectory_points_ptr, remaining_trajectory_points);

MACI_DLL_SUPPORT_FUNCTIONS(SRTProgramTrackMinorServoImpl)
