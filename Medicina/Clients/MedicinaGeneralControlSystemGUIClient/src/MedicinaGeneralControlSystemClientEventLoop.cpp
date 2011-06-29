#include <MedicinaGeneralControlSystemClientEventLoop.h>
#include <maciSimpleClient.h>
#include <ace/Time_Value.h>

void MedicinaGeneralControlSystemClientEventLoop::setSimpleClient (maci::SimpleClient * theClient)
{
  sc = theClient;
}

void MedicinaGeneralControlSystemClientEventLoop::run (void)
{
  eventEnd = false;
  ACE_Time_Value t;

  while (!eventEnd)
    {
      t.set (5L, 0L);
      sc->run (t);
    }
}

void MedicinaGeneralControlSystemClientEventLoop::stop (void)
{
  eventEnd = true;
}
