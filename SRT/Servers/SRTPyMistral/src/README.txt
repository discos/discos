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
