#include "FastQueue.h"
#include <stdlib.h>

namespace IRALibraryTest {

#define FASTQUEUE_SIZE 10000

class IRALibrary_FastQueue : public ::testing::Test {
public:
	::testing::AssertionResult FastQueue_checkLimits() {
		RecordProperty("description","check the limits of the queue");
		IRA::CFastQueue<unsigned> queue(FASTQUEUE_SIZE);
		if (!queue.isEmpty()) ::testing::AssertionFailure() << " the container should be empty";
		for (unsigned i=1;i<FASTQUEUE_SIZE;i++) {
			if (!queue.pushFront(i)) {
				return ::testing::AssertionFailure() << i << " element cannot be inserted";
			}
		}
		if (queue.pushFront(1)) ::testing::AssertionFailure() << " insertion should have failed because of container limits";
		if (queue.size()!=FASTQUEUE_SIZE) ::testing::AssertionFailure() << " the size of the container is not expected";
		return ::testing::AssertionSuccess();
	}

protected:
	virtual void SetUp() {
	}
	virtual void TearDown() {
	}
};

}
