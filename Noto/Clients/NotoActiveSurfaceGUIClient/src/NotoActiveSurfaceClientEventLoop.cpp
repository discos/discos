#include <NotoActiveSurfaceClientEventLoop.h>
#include <maciSimpleClient.h>
#include <ace/Time_Value.h>

void NotoActiveSurfaceClientEventLoop::setSimpleClient (maci::SimpleClient * theClient)
{
  sc = theClient;
}

void NotoActiveSurfaceClientEventLoop::run (void)
{
  eventEnd = false;
  ACE_Time_Value t;

	ACS_SHORT_LOG((LM_INFO, "NotoActiveSurfaceClientEventLoop %s ","UNO"));

  while (!eventEnd)
    {
      t.set (5L, 0L);
      sc->run (t);
    }
}

void NotoActiveSurfaceClientEventLoop::stop (void)
{
  eventEnd = true;
}
