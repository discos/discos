#ifndef ACTIVE_SURFACE_USD_DEVIO_H
#define ACTIVE_SURFACE_USD_DEVIO_H

/*********************************************************************/
/* OAC Osservatorio Astronomico di Cagliari                          */
/*                                                                   */
/* This code is under GNU General Public Licence (GPL).             */
/*                                                                   */
/* Who                                          when        what     */
/* Giuseppe Carboni (giuseppe.carboni@inaf.it)  2026        Redesign */
/*********************************************************************/

#include <baciDevIO.h>
#include <ASErrors.h>
#include <ActiveSurfaceUSDCore.hpp>

/**
 * Tag types used to select which USDCore accessor to call at compile time.
 * Each property instantiates USDDevIO with one of these tags.
 */
namespace USDProp {
    struct ActPos   {};
    struct Status   {};
    struct SoftVer  {};
    struct Type     {};
    struct CmdPos   {};
    struct Fmin     {};
    struct Fmax     {};
    struct Acc      {};
    struct Delay    {};
    struct UBits    {};
}

/**
 * Template DevIO for USD properties backed by USDCore.
 *
 * T    — CORBA value type (CORBA::Long or ACS::pattern)
 * Tag  — one of the USDProp:: tag types above
 */
template <typename T, typename Tag>
class USDDevIO : public DevIO<T>
{
public:
    explicit USDDevIO(ActiveSurface::USDCore& core)
        : m_core(core)
    {}

    bool initializeValue() { return false; }

    T read(ACS::Time& timestamp)
    {
        try
        {
            timestamp = getTimeStamp();
            return doRead(Tag{});
        }
        catch (const CORBA::SystemException&)
        {
            throw ASErrors::corbaErrorExImpl(__FILE__, __LINE__,
                                             "USDDevIO::read()");
        }
    }

    void write(const T& value, ACS::Time& timestamp)
    {
        try
        {
            doWrite(value, Tag{});
            timestamp = getTimeStamp();
        }
        catch (const CORBA::SystemException&)
        {
            throw ASErrors::corbaErrorExImpl(__FILE__, __LINE__,
                                             "USDDevIO::write()");
        }
    }

private:
    ActiveSurface::USDCore& m_core;

    // -----------------------------------------------------------------------
    // Read dispatch — one overload per tag
    // -----------------------------------------------------------------------
    T doRead(USDProp::ActPos)
    {
        m_core.readActPos();
        return static_cast<T>(m_core.getActPos());
    }

    T doRead(USDProp::Status)
    {
        m_core.readHWStatus();
        return static_cast<T>(m_core.getHwStatus());
    }

    T doRead(USDProp::SoftVer) { return static_cast<T>(m_core.getSoftVer());  }
    T doRead(USDProp::Type)    { return static_cast<T>(m_core.getType());     }
    T doRead(USDProp::CmdPos)  { return static_cast<T>(m_core.getCmdPos());   }
    T doRead(USDProp::Fmin)    { return static_cast<T>(m_core.getFmin());     }
    T doRead(USDProp::Fmax)    { return static_cast<T>(m_core.getFmax());     }
    T doRead(USDProp::Acc)     { return static_cast<T>(m_core.getAcc());      }
    T doRead(USDProp::Delay)   { return static_cast<T>(m_core.getDelay());    }
    T doRead(USDProp::UBits)   { return static_cast<T>(m_core.getUBits());    }

    // -----------------------------------------------------------------------
    // Write dispatch — only RW properties do something meaningful
    // -----------------------------------------------------------------------
    void doWrite(const T& v, USDProp::CmdPos) { m_core.setCmdPos(static_cast<long>(v));  }
    void doWrite(const T& v, USDProp::Fmin)   { m_core.setFmin(static_cast<long>(v));    }
    void doWrite(const T& v, USDProp::Fmax)   { m_core.setFmax(static_cast<long>(v));    }
    void doWrite(const T& v, USDProp::Acc)    { m_core.setAcc(static_cast<long>(v));     }
    void doWrite(const T& v, USDProp::Delay)  { m_core.setDelay(static_cast<long>(v));   }
    void doWrite(const T& v, USDProp::UBits)  { m_core.setUBits(static_cast<long>(v));   }

    // RO properties: write is a no-op (BACI calls it on set_sync, ignore)
    void doWrite(const T&, USDProp::ActPos)  {}
    void doWrite(const T&, USDProp::Status)  {}
    void doWrite(const T&, USDProp::SoftVer) {}
    void doWrite(const T&, USDProp::Type)    {}
};

#endif // ACTIVE_SURFACE_USD_DEVIO_H
