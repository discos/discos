********************
How to run the tests
********************

Start ACS::

   $ acsStart

From the Supervisor directory, start the containers::

   $ bin/supervisor_startcontainers.sh

Run all tests::
    
    .. code-block: bash

    $ cd test
    $ python -m unittest discover

Execute a particular test::

.. code-block: bash

    $ python cli/test_cli.py

You can also use coverage::

   $ ./run_coverage

To stop the containers::

   $ bin/supervisor_stopcontainers.sh

To stop ACS::

   $ killACS
   $ acsStop
