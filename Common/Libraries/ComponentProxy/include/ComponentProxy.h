#ifndef COMPONENTPROXY_H
#define COMPONENTPROXY_H

#include <string>

#include <boost/shared_ptr.hpp>

#include <acsContainerServices.h>
#include <ComponentErrors.h>

#include <CustomLoggerUtils.h>
#include <IRA>

#define PROXY_STRINGIFY(x) #x
#define PROXY_PASTE(a, b) a##b
#define PROXY_COMPONENT(COMPONENT_TYPE) \
typedef ComponentProxy<COMPONENT_TYPE, \
                       PROXY_PASTE(COMPONENT_TYPE, _var) > \
                       PROXY_PASTE(COMPONENT_TYPE, _proxy); \
typedef boost::shared_ptr<PROXY_PASTE(COMPONENT_TYPE, _proxy) > \
        PROXY_PASTE(COMPONENT_TYPE, _proxy_ptr);

/**
 * This class should be used by everyone to manipulate references to 
 * components from within other components.
 * Usage should be as simple as:
 *
 * using namespace Antenna;
 * PROXY_COMPONENT(AntennaBoss);
 * AntennaBoss_proxy m_antennaBoss("MyComponentName", m_services);
 * m_antennaBoss->yourAntennaBossMethod(methodParameters); 
 */
template <typename ComponentClass, typename ComponentVar>
class ComponentProxy
{
    public:
        ComponentProxy(const char*, maci::ContainerServices*);
        virtual ~ComponentProxy();
        void loadDefault();
        void unload();
        ComponentVar getComponentVar(){ return m_component_var;};
        ComponentVar operator*();
        ComponentVar operator->();
        void setError(){ m_error = true;};
        void resetError(){ m_error = false;};
    private:
        // We only allow to create an instance via factory methods or explicit
        // constructor
        // thus we disable copy contructor and assignment operator
        ComponentProxy(const ComponentProxy&);
        ComponentProxy& operator=(const ComponentProxy&);
        std::string m_name; 
        maci::ContainerServices *m_services;
        ComponentVar m_component_var;
        bool m_error;
};

template <typename ComponentClass, typename ComponentVar>
ComponentProxy<ComponentClass, ComponentVar>::ComponentProxy(const char * name, 
                                 maci::ContainerServices * services = NULL) :
                                 m_name(name),
                                 m_services(services),
                                 m_error(false)
{
    m_component_var = ComponentClass::_nil();
    if(m_services == NULL)
    {
        //TODO: getContainerServices on your own
    }
}

template <typename ComponentClass, typename ComponentVar>
ComponentProxy<ComponentClass, ComponentVar>::~ComponentProxy()
{
    this->unload();
}

template <typename ComponentClass, typename ComponentVar>
void
ComponentProxy<ComponentClass, ComponentVar>::loadDefault()
{
    if ((!CORBA::is_nil(m_component_var)) && (m_error)) { 
    // if reference was already taken, but an error was found 
    // dispose the reference
        try {
            m_services->releaseComponent((const char*)m_component_var->name());
            CUSTOM_LOG(LM_FULL_INFO,"ComponentLoader::loadDefault",
            //(LM_INFO, PROXY_PASTE("releasing ", PROXY_STRINGIFY(ComponentClass))));
            (LM_INFO, "releasing "));
        }catch (...) { //dispose silently...if an error...no matter
        }
        m_component_var = ComponentClass::_nil();
    }
    if (CORBA::is_nil(m_component_var)) {
    //only if it has not been retrieved yet
        try {
            m_component_var = m_services->getDefaultComponent<ComponentClass>(m_name.c_str());
            CUSTOM_LOG(LM_FULL_INFO,"ComponentLoader::loadDefault",
            //(LM_INFO, PROXY_PASTE("loading ", PROXY_STRINGIFY(ComponentClass))));
            (LM_INFO, "loading "));
            m_error = false;
        } catch (maciErrType::CannotGetComponentExImpl& ex) {
            _ADD_BACKTRACE(ComponentErrors::CouldntGetComponentExImpl,
                           Impl, ex, "ComponentLoader::loadDefault()");
            Impl.setComponentName(m_name.c_str());
            m_component_var = ComponentClass::_nil();
            throw Impl;     
        } catch (maciErrType::NoPermissionExImpl& ex) {
            _ADD_BACKTRACE(ComponentErrors::CouldntGetComponentExImpl,
                           Impl, ex, "ComponentLoader::loadDefault()");
            Impl.setComponentName(m_name.c_str());
            m_component_var = ComponentClass::_nil();
            throw Impl;     
        } catch (maciErrType::NoDefaultComponentExImpl& ex) {
            _ADD_BACKTRACE(ComponentErrors::CouldntGetComponentExImpl,
                           Impl, ex, "ComponentLoader::loadDefault()");
            Impl.setComponentName(m_name.c_str());
            m_component_var = ComponentClass::_nil();
            throw Impl;     
        }
    }
}

template <typename ComponentClass, typename ComponentVar>
void
ComponentProxy<ComponentClass, ComponentVar>::unload()
{
    if (!CORBA::is_nil(m_component_var)) { 
        try {
            m_services->releaseComponent((const char*)m_component_var->name());
            CUSTOM_LOG(LM_FULL_INFO,"ComponentLoader::unload",
            //(LM_INFO, PROXY_PASTE("releasing ", PROXY_STRINGIFY(ComponentClass))));
            (LM_INFO, "releasing "));
        }catch (maciErrType::CannotReleaseComponentExImpl& ex) {
            _ADD_BACKTRACE(ComponentErrors::CouldntReleaseComponentExImpl,
                           Impl,ex,"ComponentLoader::unload()");
            Impl.setComponentName(m_name.c_str());
            Impl.log(LM_WARNING);
        }catch (...) { 
            _EXCPT(ComponentErrors::UnexpectedExImpl, impl,
                   "ComponentLoader::unload()");
            impl.log(LM_WARNING);
        }
        m_component_var = ComponentClass::_nil();
    } 
}

template <typename ComponentClass, typename ComponentVar>
ComponentVar
ComponentProxy<ComponentClass, ComponentVar>::operator->()
{
    loadDefault();
    return m_component_var;
}

template <typename ComponentClass, typename ComponentVar>
ComponentVar
ComponentProxy<ComponentClass, ComponentVar>::operator*()
{
    loadDefault();
    return m_component_var;
}

#endif

