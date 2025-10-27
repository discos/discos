#include "lanStatus.h"

boost::mutex lanStatus::s_mutex;
std::map<unsigned int, std::shared_ptr<lanStatus>> lanStatus::s_map;

lanStatus& lanStatus::getInstance(const unsigned int id)
{
    boost::lock_guard<boost::mutex> lock(s_mutex);
    if(!s_map.count(id))
    {
        s_map[id] = std::shared_ptr<lanStatus>(new lanStatus(), lanStatus::Deleter{});
    }

    return *s_map.at(id);
};

std::vector<ActiveSurface::USDStatus> lanStatus::readAll()
{
    boost::unique_lock<boost::shared_mutex> lock(m_mutex);

    std::vector<ActiveSurface::USDStatus> seq;
    for(const auto& kv : m_map)
    {
        seq.push_back(kv.second);
    }
    return seq;
};

void lanStatus::write(const ActiveSurface::USDStatus& value)
{
    const unsigned long id = value.id;
    boost::shared_lock<boost::shared_mutex> rlock(m_mutex);

    auto it = m_map.find(id);
    if(it != m_map.end())
    {
        m_map[id] = value;
        return;
    }

    rlock.unlock();
    boost::unique_lock<boost::shared_mutex> wlock(m_mutex);

    m_map[id] = value;
};
