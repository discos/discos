/*******************************************************************************\
 *  Author Infos
 *  ============
 *  Name:         Marco Buttu
 *  E-mail:       mbuttu@oa-cagliari.inaf.it
 *  Personal Web: http://www.pypeople.com/
\*******************************************************************************/

#ifndef __PDOUBLESEQDEVIO__H
#define __PDOUBLESEQDEVIO__H

#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif

#include <baciDevIO.h>
#include <IRA>
#include "WPServoTalker.h"
#include <ComponentErrors.h>
#include <LogFilter.h>
#include <MSParameters.h>
#include <MinorServoErrors.h>
#include "utils.h"

using namespace IRA;

class PdoubleSeqDevIO : public DevIO<ACS::doubleSeq>
{
public:
    enum TLinkedProperty {

        ACT_POS,           /*!< the devio will be used to read the  actPos */
        CMD_POS,           /*!< the devio will be used to set the  cmdPos */
        POS_DIFF,          /*!< the devio will be used to read the  posDiff */
        ACT_ELONGATION,    /*!< the devio will be used to read the  actElongation */
        ENG_TEMPERATURE,   /*!< the devio will be used to read the  engTemperature */
        COUNTURING_ERR,    /*!< the devio will be used to read the  counturingErr */
        TORQUE_PERC,       /*!< the devio will be used to read the  torquePerce */
        ENG_CURRENT,       /*!< the devio will be used to read the  engCurrent */
        ENG_VOLTAGE,       /*!< the devio will be used to read the  engVoltage */
        DRI_TEMPERATURE,   /*!< the devio will be used to read the  driTemperature */
        UTILIZATION_PERC,  /*!< the devio will be used to read the  utilizationPerc */
        DC_TEMPERATURE,    /*!< the devio will be used to read the  dcTemperature */
        DRIVER_STATUS,     /*!< the devio will be used to read the  driverStatus */
        ERROR_CODE         /*!< the devio will be used to read the  errorCode */
    };
    
    PdoubleSeqDevIO(
            WPServoTalker *talk, 
            TLinkedProperty Property,
            CDBParameters *cdb_ptr,
            ExpireTime *expire_ptr,
            DDWORD guardInterval=1000000
            ) : m_wpServoTalk(talk), 
                m_Property(Property), 
                m_logGuard(guardInterval),
                m_cdb_ptr(cdb_ptr),
                m_expire_ptr(expire_ptr)
    {

        switch (m_Property) {

            case ACT_POS: {
                m_PropertyName=CString("Actual Position");
                break;
            }
            case CMD_POS: {
                m_PropertyName=CString("Commanded Position");
                break;
            }
            case POS_DIFF: {
                m_PropertyName=CString("Position Difference");
                break;
            }
            case ACT_ELONGATION: {
                m_PropertyName=CString("Actual Elongation");
                break;
            }
            case ENG_TEMPERATURE: {
                m_PropertyName=CString("Engine Temperature");
                break;
            }
            case COUNTURING_ERR: {
                m_PropertyName=CString("Counturing Error");
                break;
            }
            case TORQUE_PERC: {
                m_PropertyName=CString("Torque Percentage");
                break;
            }
            case ENG_CURRENT: {
                m_PropertyName=CString("Engine Current");
                break;
            }
            case ENG_VOLTAGE: {
                m_PropertyName=CString("Engine Voltage");
                break;
            }
            case DRI_TEMPERATURE: {
                m_PropertyName=CString("Driver Temperature");
                break;
            }
            case UTILIZATION_PERC: {
                m_PropertyName=CString("Utilization Percentage");
                break;
            }
            case DC_TEMPERATURE: {
                m_PropertyName=CString("Drive Cabinet Temperature");
                break;
            }
            case DRIVER_STATUS: {
                m_PropertyName=CString("Driver Status");
                break;
            }
            case ERROR_CODE: {
                m_PropertyName=CString("Error Code");
                break;
            }
        }

        CString trace("PdoubleSeqDevIO::PdoubleSeqDevIO()");
        trace += m_PropertyName;
        AUTO_TRACE((const char*)trace);     
    }

    ~PdoubleSeqDevIO()
    {
        CString trace("PdoubleSeqDevIO::~PdoubleSeqDevIO()");
        trace += m_PropertyName;
        ACS_TRACE((const char*)trace);      
    }

    bool initializeValue()
    {       
        CString trace("PdoubleSeqDevIO::initializeValue()");
        trace += m_PropertyName;
        AUTO_TRACE((const char*)trace);     
        return false;
    }

    
    /**
     * Used to read the property value.
     * 
     * @param timestamp epoch when the operation completes
     * @throw ComponentErrors::PropertyErrorExImpl
     */ 
    ACS::doubleSeq read(ACS::Time &timestamp) throw (ComponentErrors::PropertyErrorExImpl)
    {
        CString trace("PdoubleSeqDevIO::read()");
        trace += m_PropertyName;
        ACS_TRACE((const char*)trace);      
        timeval now;

        try {
            switch (m_Property) {

                case ACT_POS: {
                    // if(!isValueUpToDate()) {
                    //     m_wpServoTalk->getActPos(m_expire_ptr->actPos[m_cdb_ptr->SERVO_ADDRESS], timestamp);
                    //     gettimeofday(&now, NULL);
                    //     m_expire_ptr->timeLastActPos[m_cdb_ptr->SERVO_ADDRESS] = now.tv_sec + now.tv_usec / 1000000.0;
                    // }
                    m_value = m_expire_ptr->actPos[m_cdb_ptr->SERVO_ADDRESS];
                    break;
                }

                case CMD_POS: {
                    m_value = m_expire_ptr->cmdPos[m_cdb_ptr->SERVO_ADDRESS];
                    break;
                }

                // The pos_diff is not filtered by isValueUpToDate and sets the LastActPos time
                case POS_DIFF: {
                    m_value = m_expire_ptr->posDiff[m_cdb_ptr->SERVO_ADDRESS];
                    break;
                }

                case ACT_ELONGATION: {
                    // if(!isValueUpToDate()) {
                    //     m_wpServoTalk->getActElongation(m_expire_ptr->actPos[m_cdb_ptr->SERVO_ADDRESS], timestamp);
                    //     gettimeofday(&now, NULL);
                    //     m_expire_ptr->timeLastActPos[m_cdb_ptr->SERVO_ADDRESS] = now.tv_sec + now.tv_usec / 1000000.0;
                    // }
                    m_value = m_expire_ptr->actElongation[m_cdb_ptr->SERVO_ADDRESS];
                    break;
                }

                case ENG_TEMPERATURE: {
                    if(!isValueUpToDate()) {
                        m_wpServoTalk->getParameter(
                                m_expire_ptr->engTemperature[m_cdb_ptr->SERVO_ADDRESS], 
                                timestamp, 
                                ENG_TEMPERATURE_IDX,
                                ENG_TEMPERATURE_SUB
                        );
                        gettimeofday(&now, NULL);
                        m_expire_ptr->timeLastEngTemperature[m_cdb_ptr->SERVO_ADDRESS] = now.tv_sec + now.tv_usec / 1000000.0;
                    }
                    m_value = m_expire_ptr->engTemperature[m_cdb_ptr->SERVO_ADDRESS];
                    break;
                }

                case COUNTURING_ERR: {
                    if(!isValueUpToDate()) {
                        m_wpServoTalk->getParameter(
                                m_expire_ptr->counturingErr[m_cdb_ptr->SERVO_ADDRESS], 
                                timestamp, 
                                COUNTURING_ERR_IDX,
                                COUNTURING_ERR_SUB
                        );
                        gettimeofday(&now, NULL);
                        m_expire_ptr->timeLastCounturingErr[m_cdb_ptr->SERVO_ADDRESS] = now.tv_sec + now.tv_usec / 1000000.0;
                    }
                    m_value = m_expire_ptr->counturingErr[m_cdb_ptr->SERVO_ADDRESS];
                    break;
                }

                case TORQUE_PERC: {
                    if(!isValueUpToDate()) {
                        m_wpServoTalk->getParameter(
                                m_expire_ptr->torquePerc[m_cdb_ptr->SERVO_ADDRESS], 
                                timestamp, 
                                TORQUE_PERC_IDX,
                                TORQUE_PERC_SUB
                        );
                        gettimeofday(&now, NULL);
                        m_expire_ptr->timeLastTorquePerc[m_cdb_ptr->SERVO_ADDRESS] = now.tv_sec + now.tv_usec / 1000000.0;
                    }
                    m_value = m_expire_ptr->torquePerc[m_cdb_ptr->SERVO_ADDRESS];
                    break;
                }

                case ENG_CURRENT: {
                    if(!isValueUpToDate()) {
                        m_wpServoTalk->getParameter(
                                m_expire_ptr->engCurrent[m_cdb_ptr->SERVO_ADDRESS], 
                                timestamp, 
                                ENG_CURRENT_IDX,
                                ENG_CURRENT_SUB
                        );
                        gettimeofday(&now, NULL);
                        m_expire_ptr->timeLastEngCurrent[m_cdb_ptr->SERVO_ADDRESS] = now.tv_sec + now.tv_usec / 1000000.0;
                    }
                    m_value = m_expire_ptr->engCurrent[m_cdb_ptr->SERVO_ADDRESS];
                    break;
                }

                case ENG_VOLTAGE: {
                    if(!isValueUpToDate()) {
                        m_wpServoTalk->getParameter(
                                m_expire_ptr->engVoltage[m_cdb_ptr->SERVO_ADDRESS], 
                                timestamp, 
                                ENG_VOLTAGE_IDX,
                                ENG_VOLTAGE_SUB
                        );
                        gettimeofday(&now, NULL);
                        m_expire_ptr->timeLastEngVoltage[m_cdb_ptr->SERVO_ADDRESS] = now.tv_sec + now.tv_usec / 1000000.0;
                    }
                    m_value = m_expire_ptr->engVoltage[m_cdb_ptr->SERVO_ADDRESS];
                    break;
                }

                case DRI_TEMPERATURE: {
                    if(!isValueUpToDate()) {
                        m_wpServoTalk->getParameter(
                                m_expire_ptr->driTemperature[m_cdb_ptr->SERVO_ADDRESS], 
                                timestamp, 
                                DRI_TEMPERATURE_IDX,
                                DRI_TEMPERATURE_SUB
                        );
                        gettimeofday(&now, NULL);
                        m_expire_ptr->timeLastDriTemperature[m_cdb_ptr->SERVO_ADDRESS] = now.tv_sec + now.tv_usec / 1000000.0;
                    }
                    m_value = m_expire_ptr->driTemperature[m_cdb_ptr->SERVO_ADDRESS];
                    break;
                }

                case UTILIZATION_PERC: {
                    if(!isValueUpToDate()) {
                        m_wpServoTalk->getParameter(
                                m_expire_ptr->utilizationPerc[m_cdb_ptr->SERVO_ADDRESS], 
                                timestamp, 
                                UTILIZATION_PERC_IDX,
                                UTILIZATION_PERC_SUB
                        );
                        gettimeofday(&now, NULL);
                        m_expire_ptr->timeLastUtilizationPerc[m_cdb_ptr->SERVO_ADDRESS] = now.tv_sec + now.tv_usec / 1000000.0;
                    }
                    m_value = m_expire_ptr->utilizationPerc[m_cdb_ptr->SERVO_ADDRESS];
                    break;
                }

                case DC_TEMPERATURE: {
                    if(!isValueUpToDate()) {
                        m_wpServoTalk->getParameter(
                                m_expire_ptr->dcTemperature[m_cdb_ptr->SERVO_ADDRESS], 
                                timestamp, 
                                DC_TEMPERATURE_IDX,
                                DC_TEMPERATURE_SUB,
                                true
                        );
                        gettimeofday(&now, NULL);
                        m_expire_ptr->timeLastDcTemperature[m_cdb_ptr->SERVO_ADDRESS] = now.tv_sec + now.tv_usec / 1000000.0;
                    }
                    m_value = m_expire_ptr->dcTemperature[m_cdb_ptr->SERVO_ADDRESS];
                    break;
                }

                case DRIVER_STATUS: {
                    if(!isValueUpToDate()) {
                        m_wpServoTalk->getParameter(
                                m_expire_ptr->driverStatus[m_cdb_ptr->SERVO_ADDRESS], 
                                timestamp, 
                                DRIVER_STATUS_IDX,
                                DRIVER_STATUS_SUB
                        );
                        gettimeofday(&now, NULL);
                        m_expire_ptr->timeLastDriverStatus[m_cdb_ptr->SERVO_ADDRESS] = now.tv_sec + now.tv_usec / 1000000.0;
                    }
                    m_value = m_expire_ptr->driverStatus[m_cdb_ptr->SERVO_ADDRESS];
                    break;
                }

                case ERROR_CODE: {
                    if(!isValueUpToDate()) {
                        m_wpServoTalk->getParameter(
                                m_expire_ptr->errorCode[m_cdb_ptr->SERVO_ADDRESS], 
                                timestamp, 
                                ERROR_CODE_IDX,
                                ERROR_CODE_SUB
                        );
                        gettimeofday(&now, NULL);
                        m_expire_ptr->timeLastErrorCode[m_cdb_ptr->SERVO_ADDRESS] = now.tv_sec + now.tv_usec / 1000000.0;
                    }
                    m_value = m_expire_ptr->errorCode[m_cdb_ptr->SERVO_ADDRESS];
                    break;
                }
            }
        }
        catch (ACSErr::ACSbaseExImpl& E) {
            _ADD_BACKTRACE(
                ComponentErrors::PropertyErrorExImpl, 
                dummy, 
                E, 
                "PdoubleSeqDevIO::read()"
            );
            dummy.setPropertyName((const char *)m_PropertyName);
            dummy.setReason("Property could not be read");
            _IRA_LOGGUARD_LOG_EXCEPTION(m_logGuard,dummy, LM_DEBUG);
            throw dummy;
        }               
        catch (MinorServoErrors::CommunicationErrorEx E) {
            _ADD_BACKTRACE(
                ComponentErrors::PropertyErrorExImpl, 
                dummy, 
                E, 
                "PdoubleSeqDevIO::read()"
            );
            dummy.setPropertyName((const char *)m_PropertyName);
            dummy.setReason("Answer not found in m_responses");
            _IRA_LOGGUARD_LOG_EXCEPTION(m_logGuard,dummy, LM_DEBUG);
            throw dummy;
        }               
        timestamp = getTimeStamp();  // Complition time
        return m_value;
    }

    /**
     * Used to write the value into controller.
     * 
     * @throw ComponentErrors::PropertyErrorExImpl
    */ 
    void write(const ACS::doubleSeq& value, ACS::Time& timestamp) throw (ComponentErrors::PropertyErrorExImpl)
    {
        CString trace("PdoubleSeqDevIO::write() ");
        trace += m_PropertyName;
        ACS_TRACE((const char*)trace);      
        try {
            switch (m_Property) {
 
                case CMD_POS: {
                    // Set the position
                    timestamp = getTimeStamp();
                    m_wpServoTalk->setCmdPos(value, timestamp, NOW);
                    break;
                }

                default: 
                    break;
            }
        }
        catch (ACSErr::ACSbaseExImpl& E) {
            _ADD_BACKTRACE(
                    ComponentErrors::PropertyErrorExImpl, 
                    dummy, 
                    E, 
                    "PdoubleSeqDevIO::write()"
            );
            dummy.setPropertyName((const char *)m_PropertyName);
            dummy.setReason("Property could not be write");
            _IRA_LOGGUARD_LOG_EXCEPTION(m_logGuard, dummy, LM_DEBUG);
            throw dummy;
        }               
        catch (MinorServoErrors::PositioningErrorEx E) {
            _ADD_BACKTRACE(
                ComponentErrors::PropertyErrorExImpl, 
                dummy, 
                E, 
                "PdoubleSeqDevIO::write()"
            );
            dummy.setPropertyName((const char *)m_PropertyName);
            dummy.setReason("Property could not be write");
            _IRA_LOGGUARD_LOG_EXCEPTION(m_logGuard,dummy, LM_DEBUG);
            throw dummy;
        }               
        catch (MinorServoErrors::CommunicationErrorEx E) {
            _ADD_BACKTRACE(
                ComponentErrors::PropertyErrorExImpl, 
                dummy, 
                E, 
                "PdoubleSeqDevIO::write()"
            );
            dummy.setPropertyName((const char *)m_PropertyName);
            dummy.setReason("Property could not be write: answer not found");
            _IRA_LOGGUARD_LOG_EXCEPTION(m_logGuard,dummy, LM_DEBUG);
            throw dummy;
        }               
        timestamp = getTimeStamp();  // Complition time
    }
    
private:

    WPServoTalker *m_wpServoTalk;

    TLinkedProperty m_Property;
    CString m_PropertyName;
    CLogGuard m_logGuard;
    CDBParameters *m_cdb_ptr;
    ExpireTime *m_expire_ptr;
    ACS::doubleSeq m_value;;

    bool isValueUpToDate(void) {

        timeval now;
        double actual_time, diff;
        gettimeofday(&now, NULL);
        actual_time = now.tv_sec + now.tv_usec / 1000000.0;

        switch (m_Property) {

            case ACT_POS: {
                diff = actual_time - m_expire_ptr->timeLastActPos[m_cdb_ptr->SERVO_ADDRESS];
                break;
            }
            case ENG_TEMPERATURE: {
                diff = actual_time - m_expire_ptr->timeLastEngTemperature[m_cdb_ptr->SERVO_ADDRESS];
                break;
            }
            case COUNTURING_ERR: {
                diff = actual_time - m_expire_ptr->timeLastCounturingErr[m_cdb_ptr->SERVO_ADDRESS];
                break;
            }
            case TORQUE_PERC: {
                diff = actual_time - m_expire_ptr->timeLastTorquePerc[m_cdb_ptr->SERVO_ADDRESS];
                break;
            }
            case ENG_CURRENT: {
                diff = actual_time - m_expire_ptr->timeLastEngCurrent[m_cdb_ptr->SERVO_ADDRESS];
                break;
            }
            case ENG_VOLTAGE: {
                diff = actual_time - m_expire_ptr->timeLastEngVoltage[m_cdb_ptr->SERVO_ADDRESS];
                break;
            }
            case DRI_TEMPERATURE: {
                diff = actual_time - m_expire_ptr->timeLastDriTemperature[m_cdb_ptr->SERVO_ADDRESS];
                break;
            }
            case UTILIZATION_PERC: {
                diff = actual_time - m_expire_ptr->timeLastUtilizationPerc[m_cdb_ptr->SERVO_ADDRESS];
                break;
            }
            case DC_TEMPERATURE: {
                diff = actual_time - m_expire_ptr->timeLastDcTemperature[m_cdb_ptr->SERVO_ADDRESS];
                break;
            }
            case DRIVER_STATUS: {
                diff = actual_time - m_expire_ptr->timeLastDriverStatus[m_cdb_ptr->SERVO_ADDRESS];
                break;
            }
            case ERROR_CODE: {
                diff = actual_time - m_expire_ptr->timeLastErrorCode[m_cdb_ptr->SERVO_ADDRESS];
                break;
            }

            default: return false;

        }

        return diff < m_cdb_ptr->EXPIRE_TIME;
    }

};

#endif 
