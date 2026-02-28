#ifndef USDMAP_H
#define USDMAP_H

#include <vector>
#include <map>
#include <string>
#include <boost/thread/shared_mutex.hpp>
#include <boost/thread/locks.hpp>
#include <ActiveSurfaceProxy.h>
#include <usdC.h>
#include <ZMQLibrary.hpp>
#include "Definitions.h"


namespace ZMQ = ZMQLibrary;

struct USDData {
    ActiveSurface::USD_proxy proxy;
    ActiveSurface::USDStatus status;
    std::string name;
    std::string zmqKey;
    int circle;
    int actuator;
    int sectorIndex;
    int lanIndex;

    USDData() {
        status.status = UNAV;
        status.id = 0;
        status.available = false;
        sectorIndex = -1;
        lanIndex = -1;
    }
};

class USDMap
{
public:
    USDMap();
    ~USDMap();

    USDMap(const USDMap&) = delete;
    USDMap& operator=(const USDMap&) = delete;

    void add(int circle, int actuator, int radiusIndex, int sectorIndex, int lanIndex, const std::string& name, const ActiveSurface::USD_proxy& proxy);

    void clear();

    ActiveSurface::USD_proxy get(int circle, int actuator) const;

    ActiveSurface::USD_proxy getByRadius(int radiusCircle, int radiusIndex) const;

    void updateStatus(const std::string& key, const ActiveSurface::USDStatus& status);

    int getStatusBitmaskByIndex(size_t index) const;

    bool getStatusByName(const std::string& key, ActiveSurface::USDStatus& outStatus) const;

    int getStatus(int circle, int actuator) const;

    size_t size() const;
    bool hasCircle(int circle) const;
    int countActuators(int circle) const;

    ZMQ::ZMQDictionary updateLanStatus(int sector, int lan, const ActiveSurface::USDStatusSeq* seq = nullptr);

private:
    mutable boost::shared_mutex m_mutex;
    std::vector<USDData> m_data;
    std::map<std::pair<int, int>, size_t> m_mapCoord;
    std::map<std::string, size_t> m_mapName;
    std::map<std::pair<int, int>, size_t> m_mapRadius;
    std::map<std::pair<int, int>, std::vector<size_t>> m_mapLan;
    ZMQ::ZMQDictionary calculateUSDDiff(const ActiveSurface::USDStatus& o, const ActiveSurface::USDStatus& n) const;
};

#endif
