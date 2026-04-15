#ifndef LANSTATUS_H
#define LANSTATUS_H

#include <boost/thread/shared_mutex.hpp>
#include <boost/thread/locks.hpp>
#include "ActiveSurfaceCommonS.h"

/**
 * This class implements a multiton (multi-singleton) pattern for storing the status of the USDs of all lines.
 * The LAN or USDs components must retrieve the object by calling getInstance(<lan_identifier>).
 * In this way the same object is shared between all components belonging to the given LAN.
 * Important note:
 * this class is semi-thread-safe, in the following way:
 * - reads and writes are fully thread safe
 * - multiple writes can occur at the same time provided that the inner ID of the given ActiveSurface::USDStatus is different among all the calls.
 *   This gives the opportunity to all the USDs belonging to the same line to update their status at the same time, without unnecessary locks.
 */
class lanStatus
{
public:
    /**
     * Method used to retrieve the desired lanStatus static instance.
     */
    static lanStatus& getInstance(const unsigned int id);

    /**
     * Method used to retrieve the sequence of USD statuses.
     * @return the sequence of USDStatus objects, as a std::vector
     */
    std::vector<ActiveSurface::USDStatus> readAll();

    /**
     * Method used to update the status of a USD.
     * Multiple writes can occur at the same time, provided that the inner ID fields of the given value objects are different between each other (which is our scenario).
     * @param value the USDStatus object to be inserted/updated.
     */
    void write(const ActiveSurface::USDStatus& value);

    /**
     * The singleton/multiton pattern requires assignment and copy operator to be unavailable.
     */
    lanStatus(const lanStatus&) = delete;
    lanStatus& operator=(const lanStatus&) = delete;

private:
    /**
     * Struct used for singleton deletion
     */
    struct Deleter
    {
        void operator()(lanStatus* p) const { delete p; }
    };
    friend struct Deleter;

    /**
     * The singleton/multiton pattern requires constructor and destructor to be inaccessible. The constructor is only accessed by the getInstance static method.
     */
    lanStatus() {};
    ~lanStatus() {};

    /**
     * Mutex to synchronize accesses to the inner USDStatus map
     */
    mutable boost::shared_mutex m_mutex;

    /**
     * Inner USDStatus map
     */
    std::map<unsigned int, ActiveSurface::USDStatus> m_map;

    /**
     * Map that flags updated statuses each time a write is executed
     */
    std::map<unsigned int, bool> m_changed;

    /**
     * Outer mutex used to synchronize the access to the static outer lanStatus map.
     */
    static boost::mutex s_mutex;

    /**
     * Map of lanStatus objects. Each ACS container holds the LANs of a single sector (12 LANs), so this static outer map will store all the lanStatus instances.
     */
    static std::map<unsigned int, std::shared_ptr<lanStatus>> s_map;
};

#endif
