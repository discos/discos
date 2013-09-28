#ifndef COMMONTOOLS_H_
#define COMMONTOOLS_H_

/* ************************************************************************************************************* */
/* IRA Istituto di Radioastronomia                                                                               */
/* $Id: CommonTools.h,v 1.3 2011-02-28 13:09:19 c.migoni Exp $										         */
/*                                                                                                               */
/* This code is under GNU General Public Licence (GPL).                                                          */
/*                                                                                                               */
/* Who                                                  when                       What                          */
/* Andrea Orlati(aorlati@ira.inaf.it)  08/09/2010      Creation                                                  */


#include <AntennaBossC.h>
#include <MinorServoBossC.h>
#include <String.h>
#include <SchedulerC.h>
#include <ObservatoryC.h>
#include <BackendsDefinitionsC.h>
#include <ManagmentDefinitionsC.h>
#include <Definitions.h>
#include <acsContainerServices.h>
#include <ComponentErrors.h>

class CCommonTools
{
public:
	

	/**
	 * Used to get a reference to the minor servo boss component.
	 * @param ref reference to to the minor servo boss component. if not null this does not do anything
	 * @param services pointer to the container services object
	 * @param name name of the interface of the component
	 * @param errorDetected if true and the component reference is not null the method makes a try of releasing it before acquire again the reference
	*/
	static void getMinorServoBoss(MinorServo::MinorServoBoss_var& ref,maci::ContainerServices *& services,const IRA::CString& name,bool& errorDetected) throw (
			ComponentErrors::CouldntGetComponentExImpl);

	/**
	 * used to free the reference to the minor servo boss component
	 * @param ref reference to to the antenna boss component.
	 * @param services pointer to the container services object
	 */
	static void unloadMinorServoBoss(MinorServo::MinorServoBoss_var& ref,maci::ContainerServices *& services) throw (ComponentErrors::CouldntReleaseComponentExImpl,ComponentErrors::UnexpectedExImpl);

	/**
	 * Used to get a reference to the antenna boss component.  
	 * @param ref reference to to the antenna boss component. if not null this does not do anything
	 * @param services pointer to the container services object
	 * @param name name of the interface of the component 
	 * @param errorDetected if true and the component reference is not null the method makes a try of releasing it before acquire again the reference
	*/
	static void getAntennaBoss(Antenna::AntennaBoss_var& ref,maci::ContainerServices *& services,const IRA::CString& name,bool& errorDetected) throw (ComponentErrors::CouldntGetComponentExImpl);
	
	/**
	 * used to free the reference to the antenna boss component
	 * @param ref reference to to the antenna boss component.
	 * @param services pointer to the container services object
	 */
	static void unloadAntennaBoss(Antenna::AntennaBoss_var& ref,maci::ContainerServices *& services) throw (ComponentErrors::CouldntReleaseComponentExImpl,ComponentErrors::UnexpectedExImpl);
	
	/**
	 * Used to get a reference to the scheduler component.  
	 * @param ref reference to to the sheduler component. if not null this does not do anything
	 * @param services pointer to the container services object
	 * @param name name of the interface of the component 
	 * @param errorDetected if true and the component reference is not null the method makes a try of releasing it before acquire again the reference
	 */
	static void getScheduler(Management::Scheduler_var& ref,maci::ContainerServices *& services,const IRA::CString& name,bool& errorDetected) throw (
			ComponentErrors::CouldntGetComponentExImpl);

	/**
	 * used to free the reference to the scheduler component
	 * @param ref reference to to the scheduler boss component.
	 * @param services pointer to the container services object
	 */	
	static void unloadScheduler(Management::Scheduler_var& ref,maci::ContainerServices *& services) throw (ComponentErrors::CouldntReleaseComponentExImpl,ComponentErrors::UnexpectedExImpl);

    /**
	 * Used to get a reference to the observatory component.  
	 * @param ref reference to to the observatory component. if not null this does not do anything
	 * @param services pointer to the container services object
	 * @param name name of the interface of the component 
	 * @param errorDetected if true and the component reference is not null the method makes a try of releasing it before acquire again the reference
	*/
	static void getObservatory(Antenna::Observatory_var& ref,maci::ContainerServices *& services,const IRA::CString& name,bool& errorDetected) throw (ComponentErrors::CouldntGetComponentExImpl);
	
	/**
	 * used to free the reference to the antenna boss component
	 * @param ref reference to to the antenna boss component.
	 * @param services pointer to the container services object
	 */
	static void unloadObservatory(Antenna::Observatory_var& ref,maci::ContainerServices *& services) throw (ComponentErrors::CouldntReleaseComponentExImpl,ComponentErrors::UnexpectedExImpl);
};


#endif

