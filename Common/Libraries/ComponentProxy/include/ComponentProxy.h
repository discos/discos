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
#define PROXY_PASTE2(a, b) a##PROXY_STRINGIFY(b)
#define PROXY_COMPONENT(COMPONENT_TYPE)                                \
typedef ComponentProxy<COMPONENT_TYPE,                                 \
                       PROXY_PASTE(COMPONENT_TYPE, _var) >             \
                       PROXY_PASTE(COMPONENT_TYPE, _proxy);

/**
 * This class should be used by everyone to manipulate references to 
 * components from within other components.
 * Usage should be as simple as:
 *
 * using namespace Antenna;
 * PROXY_COMPONENT(AntennaBoss);
 * AntennaBoss_proxy m_antennaBoss("MyComponentName", m_services);
 * m_antennaBoss->yourAntennaBossMethod(methodParameters); 
 * 
 * The Proxy operator '.' permits access to proxy methods:
 * .setComponentName();
 * .setError();
 * .resetError();
 * .loadDefault();
 * .unload();
 * .getComponentVar();
 *
 * While the Proxy Operator '->' gives direct access to the inner
 * object methods.
 *
 * In this same library a set of files define some commonly used proxies, for
 * example in you source code a simple:
 * #include <AntennaBossProxy.h>
 * will give immediate access to Antenna::AntennaBoss_proxy type 
 * 
 * Defining new proxies should follow the simple AntennaBoss example and should
 * be done in this module.
 */
template <typename ComponentClass, typename ComponentVar>
class ComponentProxy
{
    public:
        ComponentProxy();
        ComponentProxy(const char*, maci::ContainerServices*);
        virtual ~ComponentProxy();
        void loadDefault() throw (ComponentErrors::CouldntGetComponentExImpl);
        void unload();
        ComponentVar getComponentVar(){ return m_component_var;};
        ComponentVar operator->() throw (ComponentErrors::CouldntGetComponentExImpl);
        void setError(){ m_error = true;};
        void resetError(){ m_error = false;};
        bool isError(){ return m_error;};
        void setComponentName(const char*);
        void setContainerServices(maci::ContainerServices*);
    private:
        // We only allow to create an instance via factory methods or explicit
        // constructor thus we disable copy contructor and assignment operator
        ComponentProxy(const ComponentProxy&);
        ComponentProxy& operator=(const ComponentProxy&);
        std::string m_name; 
        maci::ContainerServices *m_services;
        ComponentVar m_component_var;
        bool m_error;
};

template <typename ComponentClass, typename ComponentVar>
ComponentProxy<ComponentClass, ComponentVar>::ComponentProxy() :
                                 m_name(""),
                                 m_services(NULL),
                                 m_error(false)
{
    m_component_var = ComponentClass::_nil();
}

template <typename ComponentClass, typename ComponentVar>
ComponentProxy<ComponentClass, ComponentVar>::ComponentProxy(const char * name,
                                 maci::ContainerServices * services) :
                                 m_name(name),
                                 m_services(services),
                                 m_error(false)
{
    m_component_var = ComponentClass::_nil();
}

template <typename ComponentClass, typename ComponentVar>
ComponentProxy<ComponentClass, ComponentVar>::~ComponentProxy()
{
    this->unload();
}

template <typename ComponentClass, typename ComponentVar>
void
ComponentProxy<ComponentClass, ComponentVar>::loadDefault()
throw (ComponentErrors::CouldntGetComponentExImpl)
{
    if(m_name == "")
    {
        CUSTOM_LOG(LM_FULL_INFO, 
                   "ComponentLoader::loadDefault",
                   (LM_WARNING, "Trying to load component without name " )
                   );
        _EXCPT(ComponentErrors::CouldntGetComponentExImpl, impl,
                   "ComponentLoader::loadDefault()");
        throw impl;
        return;
    }
    if(m_services == NULL)
    {
        CUSTOM_LOG(LM_FULL_INFO, 
                   "ComponentLoader::loadDefault",
                   (LM_WARNING, "Trying to load component without Container Services " )
                   );
        _EXCPT(ComponentErrors::CouldntGetComponentExImpl, impl,
                   "ComponentLoader::loadDefault()");
        throw impl;
        return;
    }
    if ((!CORBA::is_nil(m_component_var)) && (m_error)) { 
    // if reference was already taken, but an error was found 
    // dispose the reference
        try {
            m_services->releaseComponent((const char*)m_component_var->name());
            CUSTOM_LOG(LM_FULL_INFO, 
                       "ComponentLoader::loadDefault",
                       (LM_DEBUG, ("releasing " + this->m_name).c_str())
                       );
        }catch (...) { //dispose silently...if an error...no matter
        }
        m_component_var = ComponentClass::_nil();
    }
    if (CORBA::is_nil(m_component_var)) {
    //only if it has not been retrieved yet
        try {
            m_component_var = m_services->getDefaultComponent<ComponentClass>(m_name.c_str());
            CUSTOM_LOG(LM_FULL_INFO, 
                       "ComponentLoader::loadDefault",
                       (LM_DEBUG, ("loading " + this->m_name).c_str())
                       );
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
            CUSTOM_LOG(LM_FULL_INFO, 
                       "ComponentLoader::loadDefault",
                       (LM_DEBUG, ("releasing " + this->m_name).c_str())
                       );
        }catch (maciErrType::CannotReleaseComponentExImpl& ex) {
            _ADD_BACKTRACE(ComponentErrors::CouldntReleaseComponentExImpl,
                           Impl,ex,"ComponentLoader::unload()");
            Impl.setComponentName(m_name.c_str());
            Impl.log(LM_WARNING);
        }catch (...) { 
            _EXCPT(ComponentErrors::UnexpectedExImpl, impl,
                   "ComponentLoader::unload()");
            CUSTOM_EXCPT_LOG(impl, LM_WARNING);
        }
        m_component_var = ComponentClass::_nil();
    } 
}

template <typename ComponentClass, typename ComponentVar>
void
ComponentProxy<ComponentClass, ComponentVar>::setComponentName(const char* name)
{
    if(name == "")
        m_name = std::string(name);
    else{
        //TODO: throw exception?
    }
}

template <typename ComponentClass, typename ComponentVar>
void
ComponentProxy<ComponentClass, ComponentVar>::setContainerServices(
    maci::ContainerServices* services)
{
    if(m_services != NULL)
        m_services = services;
    else{
        //TODO: throw exception?
    }
}

template <typename ComponentClass, typename ComponentVar>
ComponentVar
ComponentProxy<ComponentClass, ComponentVar>::operator->()
throw (ComponentErrors::CouldntGetComponentExImpl)
{
    loadDefault();
    return m_component_var.out();
}


#endif

