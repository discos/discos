#include "bulkDataZMQConfiguration.h"

using namespace bulkdataZMQImpl;

// This will only allow source files from /src to instantiate the template with maci::ContainerServices
// Source files under /test are not affected by this
template class CZMQConfiguration<maci::ContainerServices>;