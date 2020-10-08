#ifndef MED_C_BAND_LOCAL_OSCILLATOR_H
#define MED_C_BAND_LOCAL_OSCILLATOR_H

#include <IRA>

/**
 * @brief This class enclose both mixer stages into a unique interface
 */
class MixerOperator{

    public:

        /**
         * @brief Construct a new Mixer Operator object
         * @details devices initialization, set default freq
         * @param[in] p_defaults Default mixer values
         * @param[in] p_mins Minimal values 
         * @param[in] p_defaults Maximum values
         */
        MixerOperator(const ACS::doubleSeq& p_defaults, 
                    const ACS::doubleSeq& p_mins,
                    const ACS::doubleSeq& p_maxs);

        /**
         * @brief Destroy the Mixer Operator object
         * 
         */
        ~MixerOperator();

        /**
         * @brief Set LO equivalent value to the 2 stages LOs
         * 
         * @param p_value 
         * @return true Value is valid         
         */
        bool SetValue(double p_value);

        /**
         * @brief Get the equivalent LO value
         * 
         * @return double LO value
         */
        double GetValue();


    private:


};

#endif