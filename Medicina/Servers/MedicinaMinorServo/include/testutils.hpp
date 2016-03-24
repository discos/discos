#ifndef MEDMINORSERVOTESTUTILS_HPP
#define MEDMINORSERVOTESTUTILS_HPP

#include <stdlib.h>
#include <time.h>
#include <iostream>

#include "MedMinorServoProtocol.hpp"
#include "MedMinorServoTime.hpp"
#include "MedMinorServoGeometry.hpp"

MedMinorServoPosition get_random_position();
MedMinorServoPosition random_offset_position(MedMinorServoPosition);

double get_random_axis_value(const MedMinorServoAxis& axis);

MEDMINORSERVOSTATUS get_random_status();

MedMinorServoPosition get_random_primary_position();
MedMinorServoPosition get_random_secondary_position();
MedMinorServoPosition get_random_position_from_status();

void print_position(MedMinorServoPosition position);

void print_status(MEDMINORSERVOSTATUS status);

void print_setpos(MEDMINORSERVOSETPOS setpos);

#endif
