#ifndef MEDMINORSERVOPARAMETERS_HPP
#define MEDMINORSERVOPARAMETERS_HPP

#include <string>
#include <vector>
#include <sstream>
#include <map>

#include <IRA>
#include <ComponentErrors.h>
#include <ManagementErrors.h>
#include <ManagmentDefinitionsS.h>
//#include <macros.def>

#include "MedMinorServoGeometry.hpp"
#include "MedMinorServoOffset.hpp"

class VirtualAxis
{
    public:
        VirtualAxis(const char* name,
                  const char* unit,
                  std::vector<double> coefficients,
                  double min,
                  double max);
        ~VirtualAxis();
        std::string get_name(){ return _name;};
        std::string get_unit(){ return _unit;};
        std::vector<double> get_coefficients(){ return _coefficients;};
        double get_position(double elevation = 45.0);
    private:
        double _min, _max;
        std::string _name, _unit;
        std::vector<double> _coefficients;
};

/**
 * Parse a line specifying the axis parameters as defined in xml schema
 * @param name: the name of the axis, that is also the name of the xml tag
 * @param line: the spec line made as:
 * "unit,min,max,coeff1,coeff2,coeff3,coeff4,coeff5"
 */
VirtualAxis parseAxisLine(const char* name, const char* line);

std::vector<std::string> split(const char* line, char delim = ',');

class MedMinorServoParameters
{
    public:
        MedMinorServoParameters(const std::string name = "undefined", 
                                const bool can_track_elevation = true,
                                const bool primary_focus = false);
        virtual ~MedMinorServoParameters();
        const std::vector<std::string> get_axes_names();
        const std::vector<std::string> get_axes_units();
        /**
         * Returns the position corrected by elevation factors if the servo
         * supports elevation tracking
         */
        MedMinorServoPosition get_position(double elevation = 45.0);
        MedMinorServoPosition get_position(double elevation,
                                           MedMinorServoOffset* offset);
        void add_axis(std::string name, 
                      std::string units,
                      std::vector<double> coefficients,
                      double min,
                      double max);
        void add_axis(VirtualAxis axis);
        std::string get_name(){ return _name;};
        bool can_track_elevation(){ return _can_track_elevation;};
        bool is_primary_focus(){ return _primary_focus;};
        std::vector<std::string> getAxes(){return _axes_names;};
        std::vector<std::string> getUnits(){return _axes_units;};
        int getAxisMapping(std::string axis_name);
    private:
        std::string _name;
        bool _can_track_elevation, _primary_focus, _secondary_focus;
        std::vector<VirtualAxis> _axes;
        std::vector<std::string> _axes_names;
        std::vector<std::string> _axes_units;
};

typedef std::map<std::string, MedMinorServoParameters> MedMinorServoConfiguration;

MedMinorServoConfiguration
get_configuration_from_CDB(maci::ContainerServices* services);

#endif

