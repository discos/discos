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
		for (unsigned i=0;i<FASTQUEUE_SIZE;i++) {
			if (!queue.pushBack(i)) {
				return ::testing::AssertionFailure() << " element " << i << " cannot be inserted";
			}
			if (queue.size()!=i+1) {
				return ::testing::AssertionFailure() << " the size of the container is not expected";
			}
		}
		if (queue.pushBack(1)) return ::testing::AssertionFailure() << " insertion should have failed because of container limits";
		if (!queue.isFull()) return ::testing::AssertionFailure() << " ithe container should be full";
		return ::testing::AssertionSuccess();
	}

	::testing::AssertionResult FastQueue_checkConsistency() {
		RecordProperty("description","check the data consistency of the queue");
		IRA::CFastQueue<unsigned> queue(FASTQUEUE_SIZE);
		unsigned elem;
		// assign some elements
		for (unsigned i=0;i<1000;i++) {
			if (!queue.pushBack(i)) {
				return ::testing::AssertionFailure() << " element " << i << " cannot be inserted";
			}
		}
		for (unsigned i=0;i<500;i++) {
			if (!queue.popFront(elem)) {
				return ::testing::AssertionFailure() << " element " << i << " cannot be extracted";
			}
			if (elem!=i) {
				return ::testing::AssertionFailure() << " element " << i << " does not match with the inserted one";
			}
		}
		for (unsigned i=1000;i<2000;i++) {
			if (!queue.pushBack(i)) {
				return ::testing::AssertionFailure() << " element " << i << " cannot be inserted";
			}
		}
		for (unsigned i=500;i<2000;i++) {
			if (!queue.popFront(elem)) {
				return ::testing::AssertionFailure() << " element " << i << " cannot be extracted";
			}
			if (elem!=i) {
				return ::testing::AssertionFailure() << " element " << i << " does not match with the inserted one";
			}
		}
		return ::testing::AssertionSuccess();
	}

protected:
	virtual void SetUp() {
	}
	virtual void TearDown() {
	}
};

}
