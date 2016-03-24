*******************
Containers to start
*******************
- acsStartContainer -py AntennaBossSimContainer
- acsStartContainer -py DerotatorContainer
- acsStartContainer -py DerotatorPositionerContainer

*****************************
Content of the test directory
*****************************
Contents:

    * ``functional`` directory: contains tests
    * ``pyunit`` directory: contains tests
    * ``dependencies`` directory: third-part dependencies
    * ``DewarPositionrMockers``: user defined mockers
    * ``no_auto``: no automatic tests


Dependencies
============
The directory ``dependencies`` contains all the dependencies needed for
the tests executions. These dependencies are:

  * unittest2
  * mocker

Other requirements
==================
The ``DewarPositionerMockers`` directory is a packege containing
a mocker. To install this package::

  $ make
  $ make install

Run the tests
=============
The tests are located in two directories, ``functional`` and ``pyunit``, and
for both is mandatory that ACS is alive. In addition, the ``functional`` tests 
require the ``DewarPositioner`` to be active, while the ``pyunit``
tests will use a mocker in case the component is not available.

We can execute a particular test::

.. code-block: bash

    $ cd pyunit/positioner/
    $ ls
    test_offset.py  test_rewind.py  test_startUpdating.py  test_stop.py
    test_init.py  test_park.py    test_setup.py   test_status.py
    $ python test_offset.py 

    INFO -> component not available: we will use a mock device
        ...
    Ran 3 tests in 2.828s

    OK

or also all the tests inside a directory::
    
    .. code-block: bash

    $ cd pyunit/positioner/
    $ unit2 discover

    INFO -> component not available: we will use a mock device
    Ran 31 tests in 15.599s

    OK

