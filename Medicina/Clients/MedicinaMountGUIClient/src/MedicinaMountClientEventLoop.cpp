#include <MedicinaMountClientEventLoop.h>
#include <maciSimpleClient.h>
#include <ace/Time_Value.h>

void MedicinaMountClientEventLoop::setSimpleClient (maci::SimpleClient * theClient)
{
  sc = theClient;
}

void MedicinaMountClientEventLoop::run (void)
{
  eventEnd = false;
  ACE_Time_Value t;

  while (!eventEnd)
    {
      t.set (5L, 0L);
      sc->run (t);
    }
}

void MedicinaMountClientEventLoop::stop (void)
{
  eventEnd = true;
}
