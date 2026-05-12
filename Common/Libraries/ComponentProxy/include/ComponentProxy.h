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
                       PROXY_PASTE(COMPONENT_TYPE, _ptr) >             \
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
 * .getComponentPtr();
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
template <typename ComponentClass, typename ComponentPtr> class ComponentProxy
{
    struct ComponentHandle {
        ComponentPtr m_component_ptr;
        maci::ContainerServices* m_services;
        std::string m_name;
        std::string m_IDL;
        bool m_error;

        ComponentHandle(maci::ContainerServices* services, const char* name, const char* interface) :
            m_component_ptr(ComponentClass::_nil()), m_services(services), m_name(name ? name : ""), m_IDL(interface ? interface : ""), m_error(false) {}

        ~ComponentHandle()
        {
            release();
        }

        void release()
        {
            if(m_services && !m_name.empty() && !CORBA::is_nil(m_component_ptr))
            {
                // We check the name of the holder, it might be a hierarchical component, in this case we don't need to call release
                std::string holder = std::string(m_services->getName().c_str());
                if(!(holder.find(m_name) != std::string::npos && holder.length() > m_name.length()))
                {
                    try
                    {
                        m_services->releaseComponent(m_name.c_str());
                        ACS_LOG(LM_FULL_INFO, "~ComponentHandle", (LM_DEBUG, ("released " + m_name).c_str()));
                    }
                    catch(maciErrType::CannotReleaseComponentExImpl& ex)
                    {
                        _ADD_BACKTRACE(ComponentErrors::CouldntReleaseComponentExImpl, Impl, ex, "~ComponentHandle");
                        Impl.setComponentName(m_name.c_str());
                        Impl.log(LM_WARNING);
                    }
                    catch(...)
                    {
                        _EXCPT(ComponentErrors::UnexpectedExImpl, Impl, "~ComponentHandle");
                        Impl.log(LM_WARNING);
                    }
                }
            }

            m_component_ptr = ComponentClass::_nil();
        }

        ComponentPtr& load()
        {
            if(m_services == nullptr)
            {
                ACS_LOG(LM_FULL_INFO, "ComponentHandle::load", (LM_WARNING, "Trying to load component without Container Services"));
                _EXCPT(ComponentErrors::CouldntGetComponentExImpl, impl, "ComponentHandle::load()");
                throw impl;
            }

            if(!CORBA::is_nil(m_component_ptr) && !m_error)
            {
                return m_component_ptr;
            }

            if(m_error)
            {
                release();
                m_error = false;
            }

            try
            {
                if(!m_name.empty())
                {
                    m_component_ptr = m_services->template getComponent<ComponentClass>(m_name.c_str());
                }
                else if(!m_IDL.empty())
                {
                    // No name but IDL type was set, use getDefaultComponent and set name
                    m_component_ptr = m_services->template getDefaultComponent<ComponentClass>(m_IDL.c_str());
                    m_name = std::string(m_component_ptr->name());
                }
                else
                {
                    // No name and no interface found, cannot proceed
                    ACS_LOG(LM_FULL_INFO, "ComponentHandle::load", (LM_WARNING, "Trying to load component without name or interface type"));
                    _EXCPT(ComponentErrors::CouldntGetComponentExImpl, impl, "ComponentHandle::load()");
                    throw impl;
                }

                m_error = false;
                ACS_LOG(LM_FULL_INFO, "ComponentHandle::load", (LM_DEBUG, ("loaded " + m_name).c_str()));
            }
            catch(maciErrType::CannotGetComponentExImpl& ex)
            {
                _ADD_BACKTRACE(ComponentErrors::CouldntGetComponentExImpl, Impl, ex, "ComponentHandle::load()");
                Impl.setComponentName(m_name.c_str());
                m_error = true;
                throw Impl;
            }
            catch(maciErrType::NoPermissionExImpl& ex)
            {
                _ADD_BACKTRACE(ComponentErrors::CouldntGetComponentExImpl, Impl, ex, "ComponentHandle::load()");
                Impl.setComponentName(m_name.c_str());
                m_error = true;
                throw Impl;
            }
            catch(maciErrType::NoDefaultComponentExImpl& ex)
            {
                _ADD_BACKTRACE(ComponentErrors::CouldntGetComponentExImpl, Impl, ex, "ComponentHandle::load()");
                Impl.setComponentName(m_name.c_str());
                m_error = true;
                throw Impl;
            }

            return m_component_ptr;
        }
    };

    public:
        ComponentProxy() : m_handle(std::make_shared<ComponentHandle>(nullptr, "", "")) {}
        ComponentProxy(maci::ContainerServices* container_services) : m_handle(std::make_shared<ComponentHandle>(container_services, "", "")) {}
        ComponentProxy(const char* interface, maci::ContainerServices* container_services, const char* name = "") :
            m_handle(std::make_shared<ComponentHandle>(container_services, name, interface)) {}

        virtual ~ComponentProxy() = default;
        ComponentProxy(const ComponentProxy& other) = default;
        ComponentProxy& operator=(const ComponentProxy& other) = default;

        void load() const;
        //void unload() const;
        //ComponentPtr getComponentPtr(){ return m_component_ptr; };
        ComponentPtr& operator->() const; //throw (ComponentErrors::CouldntGetComponentExImpl);
        void setError() const { m_handle->m_error = true; };
        void resetError() const { m_handle->m_error = false; };
        bool isError() const { return m_handle->m_error; };
        void setComponentInterface(const char*);
        void setContainerServices(maci::ContainerServices*);
        void setComponentName(const char*);

    private:
        std::shared_ptr<ComponentHandle> m_handle;
};

template <typename ComponentClass, typename ComponentPtr> void ComponentProxy<ComponentClass, ComponentPtr>::load() const
{
    m_handle->load();
}

template <typename ComponentClass, typename ComponentPtr> void ComponentProxy<ComponentClass, ComponentPtr>::setComponentInterface(const char* interface)
{
    if(m_handle->m_IDL.empty())
    {
        m_handle->m_IDL = std::string(interface);
    }
    else
    {
        //TODO: throw exception?
        ACS_LOG(LM_FULL_INFO, "ComponentProxy::setComponentInterface", (LM_DEBUG, ("ComponentInterface already defined: " + m_handle->m_IDL).c_str()));
    }
}

template <typename ComponentClass, typename ComponentPtr> void ComponentProxy<ComponentClass, ComponentPtr>::setContainerServices(maci::ContainerServices* services)
{
    if(m_handle->m_services == nullptr)
    {
        m_handle->m_services = services;
    }
    else
    {
        //TODO: throw exception?
        ACS_LOG(LM_FULL_INFO, "ComponentProxy::setContainerServices", (LM_DEBUG, "ContainerServices already defined"));
    }
}

template <typename ComponentClass, typename ComponentPtr> void ComponentProxy<ComponentClass, ComponentPtr>::setComponentName(const char* name)
{
    if(m_handle->m_name.empty())
    {
        m_handle->m_name = std::string(name);
    }
    else
    {
        //TODO: throw exception?
        ACS_LOG(LM_FULL_INFO, "ComponentProxy::setComponentName", (LM_DEBUG, ("Component name already defined: " + m_handle->m_name).c_str()));
    }
}

template <typename ComponentClass, typename ComponentPtr> ComponentPtr& ComponentProxy<ComponentClass, ComponentPtr>::operator->() const
{
    return m_handle->load();
}

#endif
