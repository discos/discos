#include "SRTMinorServoImpl.h"

using namespace MinorServo;

SRTProgramTrackMinorServoImpl::SRTProgramTrackMinorServoImpl(const ACE_CString &componentName, maci::ContainerServices *containerServices) :
    SRTBaseMinorServoImpl(componentName, containerServices),
    m_tracking_queue(1500, m_virtual_axes),
    m_tracking_delta(SRTBaseMinorServoImpl::getMotionConstant(*this, "tracking_delta")),
    m_tracking_error(m_virtual_axes, 0.0),
    m_tracking(Management::MNG_FALSE),
    m_trajectory_id(0),
    m_total_trajectory_points(0),
    m_remaining_trajectory_points(0),
    m_tracking_ptr(this),
    m_trajectory_id_ptr(this),
    m_total_trajectory_points_ptr(this),
    m_remaining_trajectory_points_ptr(this),
    m_tracking_error_ptr(this)
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
        m_tracking_error_ptr = new baci::ROdoubleSeq((m_component_name + ":tracking_error").c_str(), getComponent(),
                new MSGenericDevIO<ACS::doubleSeq, std::vector<double>>(m_tracking_error), true);
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

    ACS::doubleSeq virtual_positions = m_status.getPlainVirtualPositions();
    std::vector<double> commanded_positions;

    SRTMinorServoOperativeMode operative_mode = m_status.getOperativeMode();
    if(operative_mode == OPERATIVE_MODE_PROGRAMTRACK)
    {
        try
        {
            ACS::Time last_timestamp = m_status.getTimestamp();

            // The timestamp of the read positions always corresponds to the one we're asking since they both belong to the same STATUS command answer
            // The tracking timestamp is interpolated instead
            std::pair<ACS::Time, std::vector<double>> tracking_point = m_tracking_queue.get(last_timestamp);
            commanded_positions = tracking_point.second;
            m_commanded_virtual_positions = commanded_positions;

            m_remaining_trajectory_points.store(m_tracking_queue.getRemainingPoints(last_timestamp));

            if(tracking_point.first < last_timestamp)
            {
                // We are past the last point of the trajectory, we concluded it
                m_tracking.store(Management::MNG_FALSE);
                m_tracking_queue.clear();
                m_tracking_error = std::vector<double>(m_virtual_axes, 0.0);
                return status;
            }
        }
        catch(...)
        {
            // We might get here if m_tracking_queue is empty
            // So whenever we just got a new setup or if we are past the last point inside the trajectory
            m_tracking.store(Management::MNG_FALSE);  // May be redundant but who cares?
            m_tracking_error = std::vector<double>(m_virtual_axes, 0.0);
            return status;
        }
    }
    else if(operative_mode == OPERATIVE_MODE_SETUP || operative_mode == OPERATIVE_MODE_PRESET)
    {
        commanded_positions = m_commanded_virtual_positions;
        for(size_t i = 0; i < m_virtual_axes; i++)
        {
            commanded_positions[i] += m_user_offsets[i] + m_system_offsets[i];
        }
    }
    else
    {
        m_tracking.store(Management::MNG_FALSE);
        m_tracking_error = std::vector<double>(m_virtual_axes, 0.0);
        return status;
    }

    bool is_tracking = true;

    std::transform(virtual_positions.begin(), virtual_positions.end(), commanded_positions.begin(), m_tracking_error.begin(), [](double current_pos, double commanded_pos)
    {
        return std::fabs(current_pos - commanded_pos);
    });

    for(size_t i = 0; i < m_virtual_axes; i++)
    {
        if(std::fabs(m_tracking_error[i]) > m_tracking_delta[i])
        {
            is_tracking = false;
            break;
        }
    }

    m_tracking.store(is_tracking ? Management::MNG_TRUE : Management::MNG_FALSE);

    return status;
}

bool SRTProgramTrackMinorServoImpl::setup(const char* configuration_name, CORBA::Boolean as_off)
{
    bool return_value = SRTBaseMinorServoImpl::setup(configuration_name, as_off);

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
    ACS::doubleSeq offsets = m_status.getVirtualOffsets();
    std::vector<double> coordinates_and_offsets = coordinates;

    for(size_t i = 0; i < m_virtual_axes; i++)
    {
        coordinates_and_offsets[i] += offsets[i];
        if(coordinates_and_offsets[i] < m_min[i] || coordinates_and_offsets[i] > m_max[i])
        {
            _EXCPT(MinorServoErrors::TrackingErrorExImpl, ex, (m_servo_name + "::programTrack()").c_str());
            ex.addData("Reason", "Resulting position out of range, check the offsets!");
            ex.log(LM_DEBUG);
            throw ex.getMinorServoErrorsEx();
        }
    }

    if(!m_socket.sendCommand(SRTMinorServoCommandLibrary::programTrack(m_servo_name, trajectory_id, point_id, coordinates, point_id > 0 ? 0 : IRA::CIRATools::ACSTime2UNIXEpoch(point_time))).checkOutput())
    {
        m_error_code.store(ERROR_COMMAND_ERROR);
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
    m_tracking_queue.put(point_time, coordinates_and_offsets);
}


GET_PROPERTY_REFERENCE(Management::ROTBoolean, SRTProgramTrackMinorServoImpl, m_tracking_ptr, tracking);
GET_PROPERTY_REFERENCE(ACS::ROlong, SRTProgramTrackMinorServoImpl, m_trajectory_id_ptr, trajectory_id);
GET_PROPERTY_REFERENCE(ACS::ROlong, SRTProgramTrackMinorServoImpl, m_total_trajectory_points_ptr, total_trajectory_points);
GET_PROPERTY_REFERENCE(ACS::ROlong, SRTProgramTrackMinorServoImpl, m_remaining_trajectory_points_ptr, remaining_trajectory_points);
GET_PROPERTY_REFERENCE(ACS::ROdoubleSeq, SRTProgramTrackMinorServoImpl, m_tracking_error_ptr, tracking_error);

MACI_DLL_SUPPORT_FUNCTIONS(SRTProgramTrackMinorServoImpl)
