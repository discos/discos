#ifndef __H__EngineThread__
#define __H__EngineThread__

#include <acsThread.h>

#include <CollectorThread.h>
#include <DataCollection.h>

#include "Baseband.h"
#include "LayoutCollection.h"
#include "MBFitsManager.h"

using MBFitsWriter_private::CCollectorThread;

using MBFitsWriter_private::CConfiguration;
using MBFitsWriter_private::CDataCollection;

using IRA::CSecureArea;
using IRA::CString;

using maci::ContainerServices;

namespace MBFitsWriter_private {

	class EngineThread : public ACS::Thread {
		public:
			/**
			 * Constructor().
			 * @param name_ thread name
			 * @param dataWrapper_p_ wrapper for data repository
			 * @param responseTime_ thread's heartbeat response time in 100ns unit. Default value is ThreadBase::defaultResponseTime (=1s).
			 * @param sleepTime_ thread's sleep time in 100ns unit. Default value is ThreadBase::defaultSleepTime (=100ms).
			 */
			EngineThread( const ACE_CString& name_,
										CSecureArea<CDataCollection>* dataWrapper_p_,
										const ACS::TimeInterval& responseTime_ = ThreadBase::defaultResponseTime,
										const ACS::TimeInterval& sleepTime_ = ThreadBase::defaultSleepTime );

			virtual ~EngineThread();

			/**
			 * This method is executed once when the thread starts.
			 */
			virtual void onStart();

			/**
			 * This method is executed once when the thread stops.
			 */
			virtual void onStop();

			/**
			 * This method overrides the thread implementation class. The method is executed in a loop until the thread is alive.
			 * The thread can be exited by calling ACS::ThreadBase::stop or ACS::ThreadBase::exit command.
			 */
			virtual void runLoop();

			void setConfiguration( CConfiguration* const configuration_p_ );

			void setServices( ContainerServices* const containerServices_p_ );

			void setCollectorThread( MBFitsWriter_private::CCollectorThread* const collectThread_p_ );

			/**
			 * It sets the slice of time the thread is allowed to run.
			 * @param timeSlice_ time slice given in 100 ns
			 */
			void setTimeSlice( const long timeSlice_ );

			void setLayoutCollection( const LayoutCollection& layoutCollection_ );

		private:
			typedef map<string, CDataField::TFieldType>	StringFieldType_m_t;
			typedef StringFieldType_m_t::iterator				StringFieldType_i_m_t;
			typedef StringFieldType_m_t::const_iterator	StringFieldType_ci_m_t;

			typedef map<int, CFitsWriter::TFeedHeader>	IntFeedHeader_m_t;
			typedef IntFeedHeader_m_t::iterator					IntFeedHeader_i_m_t;
			typedef IntFeedHeader_m_t::const_iterator		IntFeedHeader_ci_m_t;

			typedef map<int, Backends::TPolarization>		IntPolarization_m_t;
			typedef IntPolarization_m_t::iterator				IntPolarization_i_m_t;
			typedef IntPolarization_m_t::const_iterator	IntPolarization_ci_m_t;

			EngineThread();																													// Not implemented
			EngineThread( const EngineThread& engineThread_ );											// Not implemented

//			virtual bool operator==( const EngineThread& engineThread_ ) const;		// Not implemented
//			virtual bool operator!=( const EngineThread& engineThread_ ) const;		// Not implemented
			EngineThread& operator=( const EngineThread& engineThread_ );						// Not implemented

			bool checkTime( const ACS::Time& currentTime_ ) const;
			bool checkTimeSlot( const ACS::Time& slotStart_ ) const;

			bool isCompletionErrorFree( ACSErr::Completion_var& completion_,
																	const string methodName_,
																	const string attributeName_,
																	const CString& componentName_ ) const;
			/**
			 * It gets and updates the all the required data from the antenna subsystem.
			 */
			void collectAntennaData();

			/**
			 * It gets and updates the receiver code, and all the required data from the receviers subsystem.
			 */
			void collectReceiversData();

			bool processData( const MBFitsManager::FeBe_v_t& frontendBackendNames_,
												const Baseband::Baseband_s_t& baseBands_,
												const string basisFrameCType_,
												const double longObj_,
												const double latObj_ );

			CDBTable* loadTimeData() const;
			CDBTable* loadAntennaParameters() const;
			CDBTable* loadPointingModelCoefficients() const;

			CSecureArea<CDataCollection>* const		  m_dataWrapper_p;
			CConfiguration*												  m_configuration_p;
			ContainerServices*										  m_containerServices_p;
			MBFitsWriter_private::CCollectorThread*	m_collectThread_p;

			Antenna::AntennaBoss_var							  m_antennaBoss_p;
			Receivers::ReceiversBoss_var					  m_receiversBoss_p;

			long						m_timeSlice;

			string					m_scanName;

			MBFitsManager*	m_mbFitsManager_p;

			LayoutCollection	m_layoutCollection;
	};
};

#endif		// __H__EngineThread__
