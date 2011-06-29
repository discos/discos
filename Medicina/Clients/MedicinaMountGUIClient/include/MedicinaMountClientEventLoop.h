#include <maciSimpleClient.h>
#include <QtCore/QThread>

class MedicinaMountClientEventLoop : public QThread
{
	public:
	virtual void run(void);
	virtual void setSimpleClient(maci::SimpleClient*);
	virtual void stop(void);

	private:
	maci::SimpleClient* sc;
	bool eventEnd;
};

