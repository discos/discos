#include <MedicinaActiveSurfaceClientEventLoop.h>
#include <maciSimpleClient.h>
#include <ace/Time_Value.h>

void MedicinaActiveSurfaceClientEventLoop::setSimpleClient (maci::SimpleClient * theClient)
{
  sc = theClient;
}

void MedicinaActiveSurfaceClientEventLoop::run (void)
{
  eventEnd = false;
  ACE_Time_Value t;

	ACS_SHORT_LOG((LM_INFO, "MedicinaActiveSurfaceClientEventLoop %s ","UNO"));

  while (!eventEnd)
    {
      t.set (5L, 0L);
      sc->run (t);
    }
}

void MedicinaActiveSurfaceClientEventLoop::stop (void)
{
  eventEnd = true;
}
