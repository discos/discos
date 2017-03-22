#include "MedMinorServoParameters.hpp"
#include <math.h>
#include <boost/lexical_cast.hpp>

VirtualAxis::VirtualAxis(const char* name,
                         const char* unit,
                         std::vector<double> coefficients,
                         double min,
                         double max):
                         _min(min),
                         _max(max),
                         _name(name),
                         _unit(unit),
                         _coefficients(coefficients)
{}

VirtualAxis
parseAxisLine(const char* name, const char* line)
{
    std::vector<std::string> tokens = split(line);
    std::string unit = tokens[0];
    double min = boost::lexical_cast<double>(tokens[1]);
    double max = boost::lexical_cast<double>(tokens[2]);
    std::vector<double> coefficients;
    for(int i=3; i<8; ++i)
    {
        coefficients.push_back(boost::lexical_cast<double>(tokens[i]));
    }
    return VirtualAxis(name,
                       unit.c_str(),
                       coefficients,
                       min, 
                       max);
}

std::vector<std::string> 
split(const char* line, char delim)
{
    std::string input_line(line);
    std::stringstream ss(input_line);
    std::string item;
    std::vector<std::string> elems;
    while(std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}

VirtualAxis::~VirtualAxis(){}

double
VirtualAxis::get_position(double elevation)
{
    int exponent = 0;
    double position = 0;
    for(std::vector<double>::iterator iter = _coefficients.begin();
        iter != _coefficients.end();
        ++iter)
    {
        position += pow(elevation, exponent++) * (*iter);
    }
    if(position < _min)
    {
        //TODO: raise warning?
        position = _min;
    }
    if(position > _max)
    {
        //TODO: raise warning?
        position = _max;
    }
    return position;
}
                        

MedMinorServoParameters::MedMinorServoParameters(
    const std::string name, 
    const bool can_track_elevation,
    const bool primary_focus) : 
    _name(name),
    _can_track_elevation(can_track_elevation),
    _primary_focus(primary_focus),
    _secondary_focus(!primary_focus)
{}

MedMinorServoParameters::~MedMinorServoParameters()
{}

void
MedMinorServoParameters::add_axis(
    std::string name, 
    std::string unit,
    std::vector<double> coefficients,
    double min,
    double max)
{
    VirtualAxis _axis(name.c_str(), unit.c_str(), coefficients, min, max);
    add_axis(_axis);
}

int
MedMinorServoParameters::getAxisMapping(std::string axis_name)
{
    for(unsigned int i = 0; i < _axes_names.size(); ++i)
        if(_axes_names[i] == axis_name)
            return (int)i;
    return -1;
}

void
MedMinorServoParameters::add_axis(VirtualAxis axis)
{
    _axes.push_back(axis);
    _axes_names.push_back(axis.get_name());
    _axes_units.push_back(axis.get_unit());
}

MedMinorServoPosition
MedMinorServoParameters::get_position(double elevation)
{
    MedMinorServoPosition position;
    if(is_primary_focus())
    {
        position.mode = MED_MINOR_SERVO_PRIMARY;
        for(std::vector<VirtualAxis>::iterator iter = _axes.begin();
            iter != _axes.end();
            ++iter)
        {
            if((*iter).get_name().compare("YP") == 0){
                position.y = (*iter).get_position(elevation);
            }
            if((*iter).get_name().compare("ZP") == 0){
                position.z = (*iter).get_position(elevation);
            }
        }
    }else
    {
        position.mode = MED_MINOR_SERVO_SECONDARY;
        for(std::vector<VirtualAxis>::iterator iter = _axes.begin();
            iter != _axes.end();
            ++iter)
        {
            if((*iter).get_name().compare("X") == 0){
                position.x = (*iter).get_position(elevation);
            }
            if((*iter).get_name().compare("Y") == 0){
                position.y = (*iter).get_position(elevation);
            }
            if((*iter).get_name().compare("Z") == 0){
                position.z = (*iter).get_position(elevation);
            }
            if((*iter).get_name().compare("THETA_X") == 0){
                position.theta_x = (*iter).get_position(elevation);
            }
            if((*iter).get_name().compare("THETA_Y") == 0){
                position.theta_y = (*iter).get_position(elevation);
            }
        } //end for
    } //end else
    return position;
}

MedMinorServoPosition
MedMinorServoParameters::get_position(double elevation, 
                                      MedMinorServoOffset *offset)
{
    MedMinorServoPosition position, offset_position;
    position = get_position(elevation);
    offset_position = offset->getOffsetPosition();
    return position + offset_position;
}

MedMinorServoConfiguration
get_configuration_from_CDB(maci::ContainerServices* services)
{
    MedMinorServoConfiguration configuration;
    IRA::CError error;
    error.Reset();
    IRA::CDBTable minor_servo_table(services,
                               "MinorServo",
                               "DataBlock/MinorServoParameters");
    if(!minor_servo_table.addField(error, "code", IRA::CDataField::STRING))
        error.setExtra("code field not found", 0);
    if(!minor_servo_table.addField(error, "primary", IRA::CDataField::LONGLONG))
        error.setExtra("primary field not found", 0);
    if(!minor_servo_table.addField(error, "trackel", IRA::CDataField::LONGLONG))
        error.setExtra("trackel field not found", 0);
    if(!minor_servo_table.addField(error, "Xaxis", IRA::CDataField::STRING))
        error.setExtra("Xaxis field not found", 0);
    if(!minor_servo_table.addField(error, "Yaxis", IRA::CDataField::STRING))
        error.setExtra("Yaxis field not found", 0);
    if(!minor_servo_table.addField(error, "Zaxis", IRA::CDataField::STRING))
        error.setExtra("Zaxis field not found", 0);
    if(!minor_servo_table.addField(error, "THETAXaxis", IRA::CDataField::STRING))
        error.setExtra("THETAXaxis field not found", 0);
    if(!minor_servo_table.addField(error, "THETAYaxis", IRA::CDataField::STRING))
        error.setExtra("THETAYaxis field not found", 0);
    if(!minor_servo_table.addField(error, "YPaxis", IRA::CDataField::STRING))
        error.setExtra("YPaxis field not found", 0);
    if(!minor_servo_table.addField(error, "ZPaxis", IRA::CDataField::STRING))
        error.setExtra("ZPaxis field not found", 0);

    if (!error.isNoError()) {
        _EXCPT_FROM_ERROR(ComponentErrors::IRALibraryResourceExImpl,dummy,error);
        dummy.setCode(error.getErrorCode());
        dummy.setDescription((const char*)error.getDescription());
        throw dummy;
    }
    if(!minor_servo_table.openTable(error)){
        _EXCPT_FROM_ERROR(ComponentErrors::CDBAccessExImpl, dummy, error);
        throw dummy;
    }
    minor_servo_table.First();
    bool can_track_elevation, primary_focus;
    long long tmp;
    for(int i=0; i<minor_servo_table.recordCount(); ++i)
    {
        tmp = minor_servo_table["primary"]->asLongLong();
        primary_focus = true ? (tmp == 1) : false;
        tmp = minor_servo_table["trackel"]->asLongLong();
        can_track_elevation = true ? (tmp == 1) : false;
        MedMinorServoParameters parameters((const char*)minor_servo_table["code"]->asString(),
                                can_track_elevation,
                                primary_focus);
        if(primary_focus)
        {
            parameters.add_axis(
                parseAxisLine(
                    "YP",
                    (const char*)minor_servo_table["YPaxis"]->asString()
                )
            );
            parameters.add_axis(
                parseAxisLine(
                    "ZP",
                    (const char*)minor_servo_table["ZPaxis"]->asString()
                )
            );
        }else{ //secondary focus
            parameters.add_axis(
                parseAxisLine(
                    "X",
                    (const char*)minor_servo_table["Xaxis"]->asString()
                )
            );
            parameters.add_axis(
                parseAxisLine(
                    "Y",
                    (const char*)minor_servo_table["Yaxis"]->asString()
                )
            );
            parameters.add_axis(
                parseAxisLine(
                    "Z",
                    (const char*)minor_servo_table["Zaxis"]->asString()
                )
            );
            parameters.add_axis(
                parseAxisLine(
                    "THETA_X",
                    (const char*)minor_servo_table["THETAXaxis"]->asString()
                )
            );
            parameters.add_axis(
                parseAxisLine(
                    "THETA_Y",
                    (const char*)minor_servo_table["THETAYaxis"]->asString()
                )
            );
        }
        configuration[parameters.get_name()] = parameters;
        CUSTOM_LOG(LM_FULL_INFO,
                   "MinorServo::MinorServoParameters::get_configuration_from_CDB",
                   (LM_DEBUG, "Read configuration: %s", (parameters.get_name()).c_str()));
        minor_servo_table.Next();
    }
    return configuration;
}

