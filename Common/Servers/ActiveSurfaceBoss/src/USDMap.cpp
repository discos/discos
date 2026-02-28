#include "USDMap.h"
#include <stdexcept>

USDMap::USDMap() {}
USDMap::~USDMap() {}

void USDMap::add(int circle, int actuator, int radiusIndex, int sectorIndex, int lanIndex, const std::string& name, const ActiveSurface::USD_proxy& proxy)
{
    boost::unique_lock<boost::shared_mutex> lock(m_mutex);

    USDData data;
    data.proxy = proxy;
    data.name = name;
    char buf[16];
    std::snprintf(buf, sizeof(buf), "USD%02d", circle);
    data.zmqKey = buf;
    data.circle = circle;
    data.actuator = actuator;
    data.sectorIndex = sectorIndex;
    data.lanIndex = lanIndex;
    data.status.id = circle;

    size_t index = m_data.size();
    m_data.push_back(data);

    m_mapCoord[std::make_pair(circle, actuator)] = index;
    m_mapName[name] = index;
    m_mapRadius[std::make_pair(circle, radiusIndex)] = index;
    m_mapLan[std::make_pair(sectorIndex, lanIndex)].push_back(index);
}

void USDMap::clear()
{
    boost::unique_lock<boost::shared_mutex> lock(m_mutex);
    m_data.clear();
    m_mapCoord.clear();
    m_mapName.clear();
    m_mapRadius.clear();
}

ActiveSurface::USD_proxy USDMap::get(int circle, int actuator) const
{
    boost::shared_lock<boost::shared_mutex> lock(m_mutex);
    auto it = m_mapCoord.find(std::make_pair(circle, actuator));
    if (it != m_mapCoord.end()) {
        return m_data[it->second].proxy;
    }
    throw std::out_of_range("USDProxy not found (Standard Coords)");
}

ActiveSurface::USD_proxy USDMap::getByRadius(int radiusCircle, int radiusIndex) const
{
    boost::shared_lock<boost::shared_mutex> lock(m_mutex);
    auto it = m_mapRadius.find(std::make_pair(radiusCircle, radiusIndex));
    if (it != m_mapRadius.end()) {
        return m_data[it->second].proxy;
    }
    throw std::out_of_range("USDProxy not found (Radius Coords)");
}

void USDMap::updateStatus(const std::string& key, const ActiveSurface::USDStatus& status)
{
    boost::unique_lock<boost::shared_mutex> lock(m_mutex);
    auto it = m_mapName.find(key);
    if (it != m_mapName.end()) {
        m_data[it->second].status = status;
    }
}

int USDMap::getStatusBitmaskByIndex(size_t index) const
{
    boost::shared_lock<boost::shared_mutex> lock(m_mutex);
    if (index < m_data.size()) {
        return m_data[index].status.status;
    }
    return UNAV;
}

bool USDMap::getStatusByName(const std::string& key, ActiveSurface::USDStatus& outStatus) const
{
    boost::shared_lock<boost::shared_mutex> lock(m_mutex);
    auto it = m_mapName.find(key);
    if (it != m_mapName.end()) {
        outStatus = m_data[it->second].status;
        return true;
    }
    return false;
}

int USDMap::getStatus(int circle, int actuator) const
{
    boost::shared_lock<boost::shared_mutex> lock(m_mutex);

    auto it = m_mapCoord.find(std::make_pair(circle, actuator));

    if (it != m_mapCoord.end()) {
        return m_data[it->second].status.status;
    }

    return UNAV;
}

size_t USDMap::size() const
{
    boost::shared_lock<boost::shared_mutex> lock(m_mutex);
    return m_data.size();
}

bool USDMap::hasCircle(int circle) const
{
    boost::shared_lock<boost::shared_mutex> lock(m_mutex);
    for(const auto& elem : m_mapCoord) {
        if (elem.first.first == circle) return true;
    }
    return false;
}

int USDMap::countActuators(int circle) const
{
    boost::shared_lock<boost::shared_mutex> lock(m_mutex);
    int count = 0;
    for(const auto& elem : m_mapCoord) {
        if (elem.first.first == circle) count++;
    }
    return count;
}

ZMQ::ZMQDictionary USDMap::updateLanStatus(int sector, int lan, const ActiveSurface::USDStatusSeq* seq)
{
    ZMQ::ZMQDictionary lanDiff;
    boost::unique_lock<boost::shared_mutex> lock(m_mutex);

    auto it = m_mapLan.find(std::make_pair(sector, lan));
    if(it == m_mapLan.end())
    {
        return lanDiff;
    }

    for(size_t idx : it->second)
    {
        USDData& data = m_data[idx];
        ActiveSurface::USDStatus targetStatus;
        bool useRealData = false;

        if(seq && seq->length() > 0)
        {
            for(size_t k = 0; k < seq->length(); k++)
            {
                if((*seq)[k].id == data.circle)
                {
                    targetStatus = (*seq)[k];
                    useRealData = true;
                    break;
                }
            }
        }
        if(!useRealData)
        {
            targetStatus.id = data.circle;
            targetStatus.status = UNAV;
            targetStatus.available = false;
        }

        ZMQ::ZMQDictionary d = calculateUSDDiff(data.status, targetStatus);
        if(!d.empty())
        {
            lanDiff[data.zmqKey] = d;
        }
        data.status = targetStatus;
    }
    return lanDiff;
}

ZMQ::ZMQDictionary USDMap::calculateUSDDiff(const ActiveSurface::USDStatus& o, const ActiveSurface::USDStatus& n) const
{
    ZMQ::ZMQDictionary diff;
    bool initialize = (o.status == UNAV && !o.available);

    if(initialize || o.available          != n.available)           diff["available"] = n.available;
    if(!n.available)                                                return diff;
    if(initialize || o.accelerationFactor != n.accelerationFactor)  diff["accelerationFactor"] = n.accelerationFactor;
    if(initialize || o.commandedPosition  != n.commandedPosition)   diff["commandedPosition"] = n.commandedPosition;
    if(initialize || o.currentPosition    != n.currentPosition)     diff["currentPosition"] = n.currentPosition;
    if(initialize || o.delay              != n.delay)               diff["delay"] = n.delay == 255 ? -1 : 512 * n.delay;
    if(initialize || o.maximumFrequency   != n.maximumFrequency)    diff["maximumFrequency"] = n.maximumFrequency / 10;
    if(initialize || o.minimumFrequency   != n.minimumFrequency)    diff["minimumFrequency"] = n.minimumFrequency / 10;
    if(initialize || o.softwareVersion    != n.softwareVersion)     diff["softwareVersion"] = std::to_string((n.softwareVersion >> 4) & 0xF) + "." + std::to_string(n.softwareVersion & 0xF);
    if(initialize || o.type               != n.type)                diff["USDType"] = n.type == 0x20 ? "USD50xxx" : "USD60xxx";
    if(initialize || o.status             != n.status)
    {
        diff["calibrated"] = (n.status & CAL) != 0;
        diff["enabled"] = (n.status & ENBL) != 0;
        diff["running"] = (n.status & MRUN) != 0;
    }

    return diff;
}
