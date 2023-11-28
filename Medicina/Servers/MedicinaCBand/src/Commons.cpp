#include <IRA>

namespace Helpers{
	    
     double linearFit(double *X,double *Y,const WORD& size,double x){
     	int low=-1,high=-1;
    	for (WORD j=0;j<size;j++) {
        if (x==X[j]) {
            return Y[j];
        }
        else if (x<X[j]) {
            if (high==-1) high=j;
            else if (X[j]<X[high]) high=j;
        }
        else if (x>X[j]) { // X value is lower
            if (low==-1) low=j;
            else if (X[j]>X[low]) low=j;
        }
    	}
    	if ((high!=-1) && (low!=-1)) {
        double slope=X[low]-X[high];
        return ((x-X[high])/slope)*Y[low]-((x-X[low])/slope)*Y[high];
    	}else if (high==-1) {
        return Y[low];
    	}else if (low==-1) {
        return Y[high];
    	}else return 0.0; //this will never happen if size!=0
    }

    /**
     *  Convert the voltage value of the vacuum to mbar
     */
    double voltage2mbar(double voltage) { return(pow(10, 1.5 * voltage - 12)); }

    /**
     *  Convert the voltage value of the temperatures to Kelvin
     */
    double voltage2Kelvin(double voltage) {
        return voltage < 1.12 ?  (660.549422889947 * pow(voltage, 6)) - (2552.334255456860 * pow(voltage, 5)) + (3742.529989384570 * pow(voltage, 4))
            - (2672.656926956470 * pow(voltage, 3)) + (947.905578508975 * pow(voltage, 2)) - 558.351002849576 * voltage + 519.607622398508  :
            (865.747519105672 * pow(voltage, 6)) - (7271.931957100480 * pow(voltage, 5)) + (24930.666241800500 * pow(voltage, 4))
            - (44623.988512320400 * pow(voltage, 3)) + (43962.922216886600 * pow(voltage, 2)) - 22642.245121997700 * voltage + 4808.631312836750;
    }

    /**
     *  Convert the voltage value of the temperatures to Celsius (Sensor B57703-10K)
     */
    double voltage2Celsius(double voltage) 
    { return -5.9931 * pow(voltage, 5) + 40.392 * pow(voltage, 4) - 115.41 * pow(voltage, 3) + 174.67 * pow(voltage, 2) - 174.23 * voltage + 112.79; }

    /**
     *  Convert the ID voltage value to the mA value
     */
    double currentConverter(double voltage) { return(10 * voltage); }

    /**
     *  Convert the VD and VG voltage values using a right scale factor
     */
    double voltageConverter(double voltage) { return(voltage); }
    
 }