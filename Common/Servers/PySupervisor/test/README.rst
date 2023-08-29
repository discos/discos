********************
How to run the tests
********************

Start ACS and all containers::

   $ discosup

Run all tests::
    
    .. code-block: bash

    $ cd test
    $ python -m unittest discover

Execute a particular test::

.. code-block: bash

    $ python cli/test_cli.py

You can also use coverage::

   $ ./run_coverage

Stop ACS::

   $ discosdown


Procedure for manual testing
============================
To manually test the antenna stop:

* discos-simulator -start
* discosup
* objexp
* jlog
* AntennaBoss.setup(CCB)
* Mount.changeMode(PRESET, PRESET)

Now, to point close to the Sun, get the current azimuth
and elevation of the Sun (in UTC time), from here:
   
   https://aa.usno.navy.mil/data/index

You should go to section "Positions of Selected Celestial Objects"
and click on "Altitude and Azimuth of the Sun or Moon During One Day".
Set the following lat/Lon: 39.4930, 9.2451. The "hours" in the time
zone section should be "0.0". Now set this values to the mount preset:

* Mount.preset(az, el)

At some point you should get a message informing that Supervisor is
going to stop the antenna, because it is pointing close to the Sun.
When the pointing is again at safe distance from the Sun, you get
another message.
