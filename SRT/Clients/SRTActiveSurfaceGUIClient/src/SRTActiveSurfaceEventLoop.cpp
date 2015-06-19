#include <SRTActiveSurfaceEventLoop.h>
#include <maciSimpleClient.h>
#include <ace/Time_Value.h>

void eventLoop::setSimpleClient (maci::SimpleClient * theClient)
{
  sc = theClient;
}

void eventLoop::run (void)
{
  eventEnd = false;
  ACE_Time_Value t;

  while (!eventEnd)
    {
      t.set (5L, 0L);
      sc->run (t);
    }
}

void eventLoop::stop (void)
{
  eventEnd = true;
}
