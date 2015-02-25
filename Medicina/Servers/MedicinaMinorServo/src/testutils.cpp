#include "testutils.hpp"

MedMinorServoPosition
get_random_position()
{
    int offset = std::rand() % 2;
    MedMinorServoPosition position(
       -50 + 30 * offset,
       -50 + 20 * offset,
       +80 + 10 * offset,
       0.01 + 0.005 * offset,
       0.01 + 0.005 * offset,
       MED_MINOR_SERVO_SECONDARY, 
       (ACS::Time) 0
    );
    return position;
}

double 
get_random_axis_value(const MedMinorServoAxis& axis)
{
    double coeff = drand48();
    return (coeff * (axis.position_max -1 - axis.position_min + 1)) + axis.position_min + 0.1;
}

MEDMINORSERVOSTATUS
get_random_status()
{
     MEDMINORSERVOSTATUS status;
     int mode = std::rand() % 2;
     if(mode == 0)
     {
         status.mode = 0;
         status.time = MedMinorServoTime::ACSToServoNow();
         status.system_status = MED_MINOR_SERVO_STATUS_PRIMARY;
         status.pos_x_yp = get_random_axis_value(MINOR_SERVO_YP);
         status.pos_y_zp = get_random_axis_value(MINOR_SERVO_ZP);
         status.pos_z1 = 0.0;
         status.pos_z2 = 0.0;
         status.pos_z3 = 0.0;
     }else{
         status.mode = 1;
         status.time = MedMinorServoTime::ACSToServoNow();
         status.system_status = MED_MINOR_SERVO_STATUS_SECONDARY;
         status.pos_x_yp = get_random_axis_value(MINOR_SERVO_X);
         status.pos_y_zp = get_random_axis_value(MINOR_SERVO_Y);
         status.pos_z1 = get_random_axis_value(MINOR_SERVO_Z1);
         status.pos_z2 = get_random_axis_value(MINOR_SERVO_Z2);
         status.pos_z3 = get_random_axis_value(MINOR_SERVO_Z3);
     }
     return status;
}

MedMinorServoPosition
get_random_primary_position()
{
     MEDMINORSERVOSTATUS status;
     status.mode = 0;
     status.time = MedMinorServoTime::ACSToServoNow();
     status.system_status = MED_MINOR_SERVO_STATUS_PRIMARY;
     status.pos_x_yp = get_random_axis_value(MINOR_SERVO_YP);
     status.pos_y_zp = get_random_axis_value(MINOR_SERVO_ZP);
     status.pos_z1 = 0.0;
     status.pos_z2 = 0.0;
     status.pos_z3 = 0.0;
     return MedMinorServoGeometry::positionFromAxes(status);
}

MedMinorServoPosition
get_random_secondary_position()
{
     MEDMINORSERVOSTATUS status;
     status.mode = 1;
     status.time = MedMinorServoTime::ACSToServoNow();
     status.system_status = MED_MINOR_SERVO_STATUS_SECONDARY;
     status.pos_x_yp = get_random_axis_value(MINOR_SERVO_X);
     status.pos_y_zp = get_random_axis_value(MINOR_SERVO_Y);
     status.pos_z1 = get_random_axis_value(MINOR_SERVO_Z1);
     status.pos_z2 = get_random_axis_value(MINOR_SERVO_Z2);
     status.pos_z3 = get_random_axis_value(MINOR_SERVO_Z3);
     return MedMinorServoGeometry::positionFromAxes(status);
}

MedMinorServoPosition
get_random_position_from_status()
{
    MEDMINORSERVOSTATUS status = get_random_status();
    return MedMinorServoGeometry::positionFromAxes(status);
}

MedMinorServoPosition
random_offset_position(MedMinorServoPosition position)
{
    MEDMINORSERVOSETPOS setpos = MedMinorServoGeometry::positionToAxes(position);
    MEDMINORSERVOSTATUS status;
    double positive_offset = 0.4;
    double negative_offset = -0.4;
    int offset;
    switch(position.mode)
    {
        case(MED_MINOR_SERVO_PRIMARY):
            status.mode = 0;
            status.system_status = MED_MINOR_SERVO_STATUS_PRIMARY;
            offset = std::rand() % 2;
            status.pos_x_yp = (offset == 0)? (setpos.pos_x_yp + positive_offset) : (setpos.pos_x_yp + negative_offset);
            offset = std::rand() % 2;
            status.pos_y_zp = (offset == 0)? (setpos.pos_y_zp + positive_offset) : (setpos.pos_y_zp + negative_offset);
            break;
        case(MED_MINOR_SERVO_SECONDARY):
            status.mode = 1;
            status.system_status = MED_MINOR_SERVO_STATUS_SECONDARY;
            offset = std::rand() % 2;
            status.pos_x_yp = (offset == 0) ? (setpos.pos_x_yp + positive_offset) : (setpos.pos_x_yp + negative_offset);
            offset = std::rand() % 2;
            status.pos_y_zp = (offset == 0) ? (setpos.pos_y_zp + positive_offset) : (setpos.pos_y_zp + negative_offset);
            offset = std::rand() % 2;
            status.pos_z1 = (offset == 0) ? (setpos.pos_z1 + positive_offset) : (setpos.pos_z1 + negative_offset);
            offset = std::rand() % 2;
            status.pos_z2 = (offset == 0) ? (setpos.pos_z2 + positive_offset) : (setpos.pos_z2 + negative_offset);
            offset = std::rand() % 2;
            status.pos_z3 = (offset == 0) ? (setpos.pos_z3 + positive_offset) : (setpos.pos_z3 + negative_offset);
            break;
        default:
            throw MinorServoGeometryError("Cannot offset position");
    }
    return MedMinorServoGeometry::positionFromAxes(status);
}

void
print_position(MedMinorServoPosition position)
{
    IRA::CString outstring;
    IRA::CIRATools::timeToStr(position.time, outstring);
    switch(position.mode)
    {
        case(MED_MINOR_SERVO_LOCAL):
            std::cout << "ERROR:Local[\n";
            break;
        case(MED_MINOR_SERVO_INTERLOCK):
            std::cout << "ERROR:Interlock[\n";
            break;
        case(MED_MINOR_SERVO_SR_BLOCK):
            std::cout << "ERROR:SRBlock[\n";
            break;
        case(MED_MINOR_SERVO_PFR_BLOCK):
            std::cout << "ERROR:PFRBlock[\n";
            break;
        case(MED_MINOR_SERVO_SYSTEM_BLOCK):
            std::cout << "ERROR:SystemBlock[\n";
            break;
        case(MED_MINOR_SERVO_PRIMARY_DISABLED):
            std::cout << "ERROR:Primary Disabled[\n";
            break;
        case(MED_MINOR_SERVO_SECONDARY_DISABLED):
            std::cout << "ERROR:Secondary Disabled[\n";
            break;
        case(MED_MINOR_SERVO_PRIMARY):
            std::cout << "Primary[\n";
            break;
        case(MED_MINOR_SERVO_SECONDARY):
            std::cout << "Secondary[\n";
            break;
        case(MED_MINOR_SERVO_OFFSET):
            std::cout << "Offset[\n";
            break;
        case(MED_MINOR_SERVO_TRANSFER_TO_PRIMARY):
            std::cout << "TransferToPrimary[\n";
            break;
        case(MED_MINOR_SERVO_TRANSFER_TO_SECONDARY):
            std::cout << "TransferToSecondary[\n";
            break;
    }
    std::cout << "\tx: " << position.x << '\n';
    std::cout << "\ty: " << position.y << '\n';
    std::cout << "\tz: " << position.z << '\n';
    std::cout << "\ttheta_x: " << position.theta_x << '\n';
    std::cout << "\ttheta_y: " << position.theta_y << '\n';
    std::cout << "\ttime: " << position.time << " : " << outstring << '\n';
    std::cout << "]\n";
}

void
print_status(MEDMINORSERVOSTATUS status)
{
    std::cout << "Read STATUS:\n";
    std::cout << "mode: " << status.mode << '\n';
    std::cout << "system_status: " << (int)status.system_status << '\n';
    std::cout << "pos_x_yp: " << status.pos_x_yp << '\n';
    std::cout << "pos_y_zp: " << status.pos_y_zp << '\n';
    std::cout << "pos_z1: " << status.pos_z1 << '\n';
    std::cout << "pos_z2: " << status.pos_z2 << '\n';
    std::cout << "pos_z3: " << status.pos_z3 << '\n';
}

void
print_setpos(MEDMINORSERVOSETPOS setpos)
{
    std::cout << "SETPOS:\n";
    std::cout << "mode: " << setpos.mode << '\n';
    std::cout << "pos_x_yp: " << setpos.pos_x_yp << '\n';
    std::cout << "pos_y_zp: " << setpos.pos_y_zp << '\n';
    std::cout << "pos_z1: " << setpos.pos_z1 << '\n';
    std::cout << "pos_z2: " << setpos.pos_z2 << '\n';
    std::cout << "pos_z3: " << setpos.pos_z3 << '\n';
}

