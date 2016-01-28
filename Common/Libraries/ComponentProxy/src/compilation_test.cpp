#include "ComponentProxies"

/**
 * As this Library is composed by only an header file
 * this test is necessary to try and compile it!! 
 * It is used only in local so that the library won't 
 * install if the compilation does not succeed
 */

int main()
{
    Management::CustomLogger_proxy m_customLogger;
    Management::ExternalClients_proxy m_externalClientsProxy;
    Management::Scheduler_proxy m_schedulerProxy;
    Management::FitsWriter_proxy m_fitsWriterProxy;
    Management::CalibrationTool_proxy m_calibrationToolProxy;
    Antenna::AntennaBoss_proxy m_antennaBoss;
    Backends::BackendsBoss_proxy m_backendBoss;
    Backends::DBBC_proxy m_dbbcProxy;
    Backends::Holography_proxy m_holographyProxy;
    Backends::NoiseGenerator_proxy m_noiseProxy;
    Backends::TotalPower_proxy m_totalPowerProxy;
    ActiveSurface::ActiveSurfaceBoss_proxy m_acitveSurfaceBossProxy;
    MinorServo::MinorServoBoss_proxy m_minorservoBossProxy;
    MinorServo::WPServo_proxy m_wpServoProxy;
    Receivers::ReceiversBoss_proxy m_receiversBossProxy;
    Receivers::DewarPositioner_proxy m_dewarPositionerProxy;
    Receivers::LocalOscillator_proxy m_local_OscillatorProxy;
    Weather::GenericWeatherStation_proxy m_genericWeatherStationProxy;
    Antenna::Mount_proxy m_mount;
    return 0;
}

