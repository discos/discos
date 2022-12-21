* Quando i test passano, eseguirli con getComponent()
* Inviare email con istruzioni

Before running the component (using simulators)::

  $ discos-set centos_7_compatibility-srt --cdb test
  $ discos-simulator -s backend -t mistral start
  $ acsStart
  $ acsStartContainer -py SRTMistralContainer

To execute the tests do not run the simulator::

  $ discos-set centos_7_compatibility-srt --cdb test
  $ acsStart
  $ acsStartContainer -py SRTMistralContainer
  $ cd ~/centos_8_compatibility-srt/SRT/Servers/SRTPyMistral/test/cli/
  $ python test_cli.py
