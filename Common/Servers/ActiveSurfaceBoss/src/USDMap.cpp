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

ActiveSurface::USDStatusSeq* USDMap::setLanDown(int sectorIndex, int lanIndex)
{
    boost::unique_lock<boost::shared_mutex> lock(m_mutex);
    ActiveSurface::USDStatusSeq_var seq = new ActiveSurface::USDStatusSeq();

    auto it = m_mapLan.find(std::make_pair(sectorIndex, lanIndex));
    if (it != m_mapLan.end())
    {
        seq->length(it->second.size());
        unsigned int count = 0;

        for (size_t idx : it->second)
        {
            m_data[idx].status.status = UNAV;
            m_data[idx].status.available = false;
            seq[count] = m_data[idx].status;
            count++;
        }
    }
    return seq._retn();
}
