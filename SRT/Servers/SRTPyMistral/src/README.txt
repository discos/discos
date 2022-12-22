How to run the component
========================
Using simulators::

  $ discos-set centos_7_compatibility-srt --cdb test
  $ discos-simulator -s backend -t mistral start
  $ acsStart
  $ acsStartContainer -py SRTMistralContainer

How to execute the tests
========================
Important: do not start the simulator because it is automatically started
before every test and stopped right after the test.  To run the tests
move to ``SRT/Servers/SRTPyMistral/test/cli/`` and execute the following
commands::

  $ discos-set centos_7_compatibility-srt --cdb test
  $ acsStart
  $ acsStartContainer -py SRTMistralContainer
  $ python test_cli.py

First test
==========
We want to:

* run a schedule
* stop the schedule
* execute a MISTRAL target-sweep
* resume the schedule
* get a final FITS (merged by MISTRAL)

Her are the steps::

1) Start the simulator by executing::

      $ discos-simulator start

2) Startup ACS and all containers.

3) Startup the MISTAL operatorInput shell::

      $ operatorInput BACKENDS/SRTMistral

   Run the following command in the MISTRAL operatorInput shell::

      > mistralSetup
      > mistralStatus

   Execute mistralStatus several times, until the setup is done.

4) Startup the DISCOS console::

      $ discosConsole

   Run the following commands on the main operatorInput shell::

      > project=yourproject
      > setupXXX
      > startSchedule=...

   At some point::
   
      > haltSchedule

5) In the MISTRAL operatorInput::

      > mistralTargetSweep
      > mistralStatus

   Execute mistralStatus several times, until the target-sweep is done.

6) In the main operatorInput::

      > startSchedule=...,N

7) Wait for the schedule to finish and check the final FITS.
