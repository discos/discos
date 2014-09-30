********************
How to run the tests
********************

Dependencies
============
Install these dependencies::

  * unittest2
  * mocker

Other requirements
==================
Install the ``DewarPositionerMockers`` package::

  $ make install

Run the tests
=============
The ``test`` directory is structured in two branches, and for both is
mandatory that ACS is alive. In addition, the ``interface`` tests require
that the ``DewarPositioner`` must be active, while the ``submodules``
tests will use a mocker in case the component is not available.

We can execute just a particular test::

.. code-block: bash

    $ cd submodules/positioner/
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

    $ cd submodules/positioner/
    $ unit2 discover

    INFO -> component not available: we will use a mock device
    Ran 31 tests in 15.599s

    OK

.. TODO: run_all from the ``test`` directory
