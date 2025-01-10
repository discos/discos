#ifndef COMPONENTPROXY_H
#define COMPONENTPROXY_H

#include <string>

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
 * AntennaBoss_proxy m_antennaBoss("MyComponentInterfaceName", m_services);
 * m_antennaBoss->yourAntennaBossMethod(methodParameters); 
 * 
 * The Proxy operator '.' permits access to proxy methods:
 * .setComponentInterface();
 * .setError();
 * .resetError();
 * .load();
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
template <typename ComponentClass, typename ComponentVar> class ComponentProxy
{
    public:
        ComponentProxy();
        ComponentProxy(const char* interface, maci::ContainerServices* container_services);
        ComponentProxy(const char* interface, maci::ContainerServices* container_services, const char* name);
        virtual ~ComponentProxy();
        void load() const;
        void unload() const;
        //ComponentVar getComponentVar(){ return m_component_var; };
        ComponentVar& operator->() const; //throw (ComponentErrors::CouldntGetComponentExImpl);
        void setError() const { m_error = true; };
        void resetError() const { m_error = false; };
        bool isError() const { return m_error; };
        void setComponentInterface(const char*);
        void setContainerServices(maci::ContainerServices*);
        void setComponentName(const char*);
    private:
        // We only allow to create an instance via factory methods or explicit
        // constructor thus we disable copy contructor and assignment operator
        ComponentProxy(const ComponentProxy&) = delete;
        ComponentProxy& operator=(const ComponentProxy&) = delete;

        std::string m_IDL;
        mutable std::string m_name;
        maci::ContainerServices* m_services;
        mutable bool m_error;
        mutable ComponentVar m_component_var;
};

template <typename ComponentClass, typename ComponentVar> ComponentProxy<ComponentClass, ComponentVar>::ComponentProxy() :
    m_IDL(""),
    m_name(""),
    m_services(nullptr),
    m_error(false),
    m_component_var(ComponentClass::_nil())
{
}

template <typename ComponentClass, typename ComponentVar> ComponentProxy<ComponentClass, ComponentVar>::ComponentProxy(const char* interface, maci::ContainerServices * services) :
    m_IDL(interface),
    m_name(""),
    m_services(services),
    m_error(false),
    m_component_var(ComponentClass::_nil())
{
}

template <typename ComponentClass, typename ComponentVar> ComponentProxy<ComponentClass, ComponentVar>::ComponentProxy(const char* interface, maci::ContainerServices * services, const char* name) :
    m_IDL(interface),
    m_name(name),
    m_services(services),
    m_error(false),
    m_component_var(ComponentClass::_nil())
{
}

template <typename ComponentClass, typename ComponentVar> ComponentProxy<ComponentClass, ComponentVar>::~ComponentProxy()
{
    unload();
}

template <typename ComponentClass, typename ComponentVar> void ComponentProxy<ComponentClass, ComponentVar>::load() const
{
    if(m_services == nullptr)
    {
        CUSTOM_LOG(LM_FULL_INFO, "ComponentLoader::load", (LM_WARNING, "Trying to load component without Container Services"));
        _EXCPT(ComponentErrors::CouldntGetComponentExImpl, impl, "ComponentLoader::load()");
        throw impl;
    }
    if(!CORBA::is_nil(m_component_var) && isError())
    {
        // if reference was already taken, but an error was found, dispose the reference
        try
        {
            m_services->releaseComponent((const char*)m_component_var->name());
            CUSTOM_LOG(LM_FULL_INFO, "ComponentLoader::load", (LM_DEBUG, ("released " + m_name).c_str()));
        }
        catch(...)
        {
            //dispose silently...if there is an error it does not matter
        }
        m_component_var = ComponentClass::_nil();
    }

    if(CORBA::is_nil(m_component_var))
    {
        //only if it has not been retrieved yet
        try
        {
            if(m_name != "")
            {
                m_component_var = m_services->getComponent<ComponentClass>(m_name.c_str());
            }
            else if(m_IDL != "")
            {
                // No name but IDL type was set, use getDefaultComponent and set name
                m_component_var = m_services->getDefaultComponent<ComponentClass>(m_IDL.c_str());
                m_name = m_component_var->name();
            }
            else
            {
                // No name and no interface found, cannot proceed
                CUSTOM_LOG(LM_FULL_INFO, "ComponentLoader::load", (LM_WARNING, "Trying to load component without name or interface type"));
                _EXCPT(ComponentErrors::CouldntGetComponentExImpl, impl, "ComponentLoader::load()");
                throw impl;
            }

            CUSTOM_LOG(LM_FULL_INFO, "ComponentLoader::load", (LM_DEBUG, ("loaded " + m_name).c_str()));
            resetError();
        }
        catch(maciErrType::CannotGetComponentExImpl& ex)
        {
            _ADD_BACKTRACE(ComponentErrors::CouldntGetComponentExImpl, Impl, ex, "ComponentLoader::load()");
            Impl.setComponentName(m_name.c_str());
            m_component_var = ComponentClass::_nil();
            setError();
            throw Impl;
        }
        catch(maciErrType::NoPermissionExImpl& ex)
        {
            _ADD_BACKTRACE(ComponentErrors::CouldntGetComponentExImpl, Impl, ex, "ComponentLoader::load()");
            Impl.setComponentName(m_name.c_str());
            m_component_var = ComponentClass::_nil();
            setError();
            throw Impl;
        }
        catch(maciErrType::NoDefaultComponentExImpl& ex)
        {
            _ADD_BACKTRACE(ComponentErrors::CouldntGetComponentExImpl, Impl, ex, "ComponentLoader::load()");
            Impl.setComponentName(m_name.c_str());
            m_component_var = ComponentClass::_nil();
            setError();
            throw Impl;
        }
    }
}

template <typename ComponentClass, typename ComponentVar> void ComponentProxy<ComponentClass, ComponentVar>::unload() const
{
    if(!CORBA::is_nil(m_component_var))
    {
        try
        {
            m_services->releaseComponent(m_name.c_str());
            CUSTOM_LOG(LM_FULL_INFO, "ComponentLoader::unload", (LM_DEBUG, ("releasing " + m_name).c_str()));
        }
        catch(maciErrType::CannotReleaseComponentExImpl& ex)
        {
            _ADD_BACKTRACE(ComponentErrors::CouldntReleaseComponentExImpl, Impl, ex, "ComponentLoader::unload()");
            Impl.setComponentName(m_name.c_str());
            CUSTOM_EXCPT_LOG(Impl, LM_WARNING);
        }
        catch(...)
        {
            _EXCPT(ComponentErrors::UnexpectedExImpl, impl, "ComponentLoader::unload()");
            CUSTOM_EXCPT_LOG(impl, LM_WARNING);
        }

        m_component_var = ComponentClass::_nil();
    } 
}

template <typename ComponentClass, typename ComponentVar> void ComponentProxy<ComponentClass, ComponentVar>::setComponentInterface(const char* interface)
{
    if(m_IDL == "")
    {
        m_IDL = std::string(interface);
    }
    else
    {
        //TODO: throw exception?
        CUSTOM_LOG(LM_FULL_INFO, "ComponentLoader::setComponentInterface", (LM_DEBUG, ("ComponentInterface already defined: " + m_IDL).c_str()));
    }
}

template <typename ComponentClass, typename ComponentVar> void ComponentProxy<ComponentClass, ComponentVar>::setContainerServices(maci::ContainerServices* services)
{
    if(m_services == nullptr)
    {
        m_services = services;
    }
    else
    {
        //TODO: throw exception?
        CUSTOM_LOG(LM_FULL_INFO, "ComponentLoader::setContainerServices", (LM_DEBUG, "ContainerServices already defined"));
    }
}

template <typename ComponentClass, typename ComponentVar> void ComponentProxy<ComponentClass, ComponentVar>::setComponentName(const char* name)
{
    if(m_name == "")
    {
        m_name = std::string(name);
    }
    else
    {
        //TODO: throw exception?
        CUSTOM_LOG(LM_FULL_INFO, "ComponentLoader::setComponentName", (LM_DEBUG, ("Component name already defined: " + m_name).c_str()));
    }
}

template <typename ComponentClass, typename ComponentVar> ComponentVar& ComponentProxy<ComponentClass, ComponentVar>::operator->() const
{
    load();
    return m_component_var;
}

#endif
