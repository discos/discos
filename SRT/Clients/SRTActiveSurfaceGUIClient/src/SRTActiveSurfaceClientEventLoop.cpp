#include <SRTActiveSurfaceClientEventLoop.h>
#include <maciSimpleClient.h>
#include <ace/Time_Value.h>

void SRTActiveSurfaceClientEventLoop::setSimpleClient (maci::SimpleClient * theClient)
{
  sc = theClient;
}

void SRTActiveSurfaceClientEventLoop::run (void)
{
  eventEnd = false;
  ACE_Time_Value t;

  while (!eventEnd)
    {
      t.set (5L, 0L);
      sc->run (t);
    }
}

void SRTActiveSurfaceClientEventLoop::stop (void)
{
  eventEnd = true;
}
