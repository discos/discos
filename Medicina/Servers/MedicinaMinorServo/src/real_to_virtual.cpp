#include <iostream>
#include <boost/lexical_cast.hpp>
#include "MedMinorServoGeometry.hpp"
#include "MedMinorServoProtocol.hpp"
#include "testutils.hpp"

#define USAGE "./real_to_virtual X Y Z1 Z2 Z3"
int main(int argc, char *argv[])
{
    MedMinorServoPosition position;
    MEDMINORSERVOSTATUS status;
    if(!(argc == 6))
    {
        std::cout << USAGE << std::endl;
        return 1;
    }
    status.pos_x_yp = boost::lexical_cast<double>(argv[1]);
    status.pos_y_zp = boost::lexical_cast<double>(argv[2]);
    status.pos_z1 = boost::lexical_cast<double>(argv[3]);
    status.pos_z2 = boost::lexical_cast<double>(argv[4]);
    status.pos_z3 = boost::lexical_cast<double>(argv[5]);

    status.system_status = MED_MINOR_SERVO_STATUS_SECONDARY;
    status.mode = 1;
    
    position = MedMinorServoGeometry::positionFromAxes(status);
    print_position(position);
    return 0;
}

