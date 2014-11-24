# Marco Buttu <mbuttu@oa-cagliari.inaf.it> 
# Doctrings from the "Observing at the SRT with Nuraghe, Issue n.8, 21/10/14",
# by Simona Righini and Andrea Orlati"


def antennaPark():
    """
    antennaPark sends the antenna to stow position
    """

def antennaReset():
    """
    antennaReset resets the antenna status after a failure, for 
    example after the emergency stop button is released.
    """

def antennaSetup():
    """
    antennaSetup=code (CCB, KKG, ...) 
    unstows the antenna, sets it to tracking mode and configures the 
    pointing model according to the specified receiver. It does NOT 
    perform the receiver and backend setup
    """

def antennaStop():
    """
    antennaStop stops the antenna motion, if any, and changes the mount 
    mode to Stop. Activities can start again only commanding a mode change
    as antennaTrack (which does not affect the overall setup) or a new setup
    """

def antennaTrack():
    """
    antennaTrack sets the antenna to PROGRAMTRACK mode. It does not change
    the pointing model or any receiver setup
    """

def antennaUnstow():
    """
    antennaUnstow performs the unstow procedure
    """

def asSetup():
    """
    asSetup=code
    configures and enables the active surface (AS). Allowed values 
    of code:

    * SF: shaped configuration in fixed position (optimised for El=45d)
    * S:  shaped configuration in tracking - i.e. it adjusts according 
          to the observed elevation position
    * PF: parabolic configuration in fixed position (optimised for El=45d)
    * P:  parabolic configuration in tracking - i.e. it adjusts according 
          to the observed elevation position
    """

def azelOffsets():
    """
    azelOffsets=double'd',double'd' 
    sets the Az-El offsets (degrees). They are intended "on sky"

    e.g. > azelOffsets=0.5d,0.3d
    sets an azimuth offset to 0.5 degrees and the elevation offset to 
    0.3 degrees
    """

def calOn():
    """
    calOn switches the calibration mark on
    """

def calOff():
    """
    calOff switches the calibration mark off
    """

def chooseBackend():
    """
    chooseBackend=string 
    selects the backend; string can be BACKENDS/TotalPower or BACKENDS/XARCOS
    """

def chooseRecorder():
    """
    chooseRecorder=string 
    selects the backend; string can be MANAGEMENT/FitsZilla, MANAGEMENT/MBFitsWriter 
    or MANAGEMENT/Point
    """

def clearServoOffsets():
    """
    clearServoOffsets
    clears the offset of every minor servo
    """

def crossScan():
    """
    crossScan=scanFrame,span,duration 
    performs a cross-scan on the previously selected target*, along the 
    scanFrame ('eq','hor' or 'gal'), spanning span degrees in duration seconds. 
    * indicated using the track or sidereal commands
    """

def derotatorClearAutoRewindingSteps():
    """TODO"""

def derotatorClearOffset():
    """TODO"""

def derotatorGetActualSetup():
    """TODO"""

def derotatorGetAutoRewindingSteps():
    """TODO"""

def derotatorGetConfiguration():
    """TODO"""

def derotatorGetPosition():
    """TODO"""

def derotatorGetRewindingMode():
    """TODO"""

def derotatorGetRewindingStep():
    """TODO"""

def derotatorIsReady():
    """TODO"""

def derotatorIsRewinding():
    """TODO"""

def derotatorIsRewindingRequired():
    """TODO"""

def derotatorPark():
    """TODO"""

def derotatorRewind():
    """TODO"""

def derotatorSetAutoRewindingSteps():
    """TODO"""

def derotatorSetConfiguration():
    """TODO"""

def derotatorSetOffset():
    """TODO"""

def derotatorGetOffset():
    """TODO"""

def derotatorSetPosition():
    """TODO"""

def derotatorSetRewindingMode():
    """TODO"""

def derotatorSetup():
    """TODO"""

def device():
    """
    device=sect 
    computes the beamsize, taking into account the present receiver and 
    backend configurations relative to section sect
    """

def flush():
    """
    flush=N deletes the N-th element in the queue of temporized commands
    """

def flushAll():
    """
    flushAll deletes all the queue of the temporized commands
    """

def focusScan():
    """
    focusScan=span,duration
    commands a focus scan on a previously selected target (by means of 
    the track or sidereal commands).
    Parameters:

    * span: the length run on the z-axis expressed in mm, 
    * duration: the time length espressed in hh:mm:ss,

    e.g. > focusScan=60,00:01:00
    """

def getAttenuations():
    """
    getAttenuations reads the attenuation values (dB) currently configured for 
    the active sections, and lists them according to increasing section number
    """

def getTpi():
    """
    getTpi reads the signal intensity (raw counts) for the active sections, 
    and lists them according to increasing section number
    """

def goOff():
    """
    goOff=frame,offset 
    slews the antenna to an offset position, in the indicated coordinate frame 
    ('eq', 'hor' or 'gal'). The user provides the offset value (degrees only), 
    but the system automatically chooses on which axis to perform the slewing, 
    taking into account the present position of the antenna

    e.g. > goOff=eq,1.0d
    """

def goTo():
    """
    goTo=double'd',double'd' 
    sends the antenna, while in TRACKING mode, to the specified Az-El position.
    Example: goTo=180d,45d
    The arguments are always rounded in the range 0-360 and 0-90 for azimuth and 
    elevation respectively (in any case the ranges are limited to mechanical contraints). 
    The jolly character is valid and is considered as:
    keep the present value.The differences from the preset command are:
    1) once the antenna reaches the destination, the system will acknowledge 
       the "on source" status;
    2) the pointing corrections (pointing model and refraction) are applyed. 
       In case they are not required they must be turned off explicitly.
    """

def haltSchedule():
    """
    haltSchedule completes the current scan and then stops the schedule
    """

def help(command):
    """
    help(command)
    prints the `command`'s documentation. For instance:

        > help(calOn)'
        calOn switches the calibration mark on

    When used without argument, it lists all the available commands:

        > help()
        antennaPark
        antennaReset
        ...
    """
    myself =  __import__(__name__)
    try:
        cmd = getattr(myself, command)
        return cmd.__doc__
    except Exception, ex:
        print ex.message
        print 'usage: help(command)'
        print 'e.g. > help(setupCCB)'
        print 'help() (without argument) lists all the available commands'

def initialize():
    """
    initialize=code 
    (CCB, KKG, etc...) configures the backend using the default parameters relative to the
    selected receiver. It does NOT act on the receiver, pointing model or antenna mount mode
    """

def integration():
    """
    integration=N
    sets the integration time
    """

def log():
    """
    log=logfilename
    allows the user to change the default logfile (named station.log).
    logfilename have be indicated without extension.
    When schedules are run, a new logfile is automatically started, 
    and it is named after the schedule: schedulename.log.
    """

def lonlatOffsets():
    """
    lonlatOffsets=double'd',double'd' 
    sets the Galactic b-l offsets (degrees). They are intended "on sky".

    e.g. > lonlatOffsets=0.1d,0.5d
    sets the galactic longitude offset to 0.1 degrees and the galactic 
    latitude offset to 0.5 degrees
    """

def moon():
    """
    moon points the antenna to the present coordinates of the center of the Moon
    """

def preset():
    """
    preset=double'd',double'd' 
    sends the antenna, if in PRESET mode, to the specified Az-El position,
    without applying any pointing correction. This is useful when needing to point 
    to a position next to the zenith. Beware: the antenna will reach the destination 
    but no "on source" flag will be raised.
    Example: preset=180d,45d
    """

def project():
    """
    project=projectcode
    sets the project code/name (a string assigned to the project by the TAC).

    (e.g. > project=scicom)
    """

def radecOffsets():
    """
    radecOffsets=double'd',double'd' 
    sets the RA-Dec offsets (degrees). They are intended "on sky".

    e.g. > radecOffsets=0.3d,0.0d
    sets the right ascension offset to 0.3 degrees and the elevation 
    offset to 0.0 degrees
    """

def receiversMode():
    """
    receiversMode=code 
    configures the working mode of the receiver, according to its peculiar
    characteristics
    """

def receiversSetup():
    """
    receiversSetup=code 
    (CCB, KKG, etc...) configures the receiver using the default parameters. It does
    NOT act on the backend, pointing model or antenna mount mode
    """

def servoPark():
    """
    servoPark
    brakes the minor servos
    """

def servoSetup():
    """
    servoSetup=code 
    (CCB, KKG, LLP, PLP, PPP) configures the minor servos in ordet to put on focus the receiver
    """

def setAttenuation():
    """
    setAttenuation=sect,att 
    sets to att (dB) the attenuator of section sect
    """

def setLO():
    """
    setLO=freq 
    Local Oscillator frequency, in MHz (one per IF, separated by ";", usually the 
    values are identical) This LO frequency corresponds to: 
    SkyFreq(@band start) - 100 MHz when using the TPB
    """

def setSection():
    """
    setSection=sect,startFreq,bw,feed,sampleRate,bins 
    configures the backend section sect.
    """

def setServoASConfiguration():
    """
    setServoASConfiguration=code
    (ON, OFF) It enables (ON) or disable (OFF) the M2 active sufrace configuration
    """

def setServoElevationTracking():
    """
    setServoElevationTracking=code
    (ON, OFF) It enables (ON) or disable (OFF) the M2 corrections during elevation antenna movements
    """

def setServoOffset():
    """
    setServoOffset=AXIS_CODE,VALUE
    It sets the offset for a servo axis.
    Allowed AXIS_CODEs:
        SRP_TX, SRP_TY, SRP_TZ, SRP_RX, SRP_RY, SRP_RZ
        PFP_RY, PFP_TX, PFP_TZ
        GFR_RZ
        M3R_RZ

    For instance, the following command sets an offset of 5mm in the x (virtual) axis of the in the SRP:
        setServoOffset=SRP_TY,5
    """

def setupCCB():
    """
    The setup command sets the antenna mount, the minor servos, 
    the selected receiver and the default backend (TotalPower) 
    according to default parameters. 
    The antenna mode is set to ProgramTrack (allowing tracking 
    and the execution of schedules). Any time the mount mode is 
    switched to ProgramTrack, the antenna will slew and go to 
    the position which had been observed the last time the 
    ProgramTrack was active. This is normal, you can command a 
    different target or go on with other operations.
    """

def setupKKG():
    pass # Documentation appened the and of this module

def setupLLP():
    pass # Documentation appened the and of this module

def setupPPP():
    pass # Documentation appened the and of this module

def sidereal():
    """
    sidereal=sourcename,RA,Dec,epoch,sector 
    points to the supplied RA-Dec position and temporarily assigns the sourcename label to it. 
    Epoch can be '1950', '2000' or '-1', the last one meaning that the provided coordinates 
    are precessed to the observing epoch. The sector keyword forces the cable wrap
    sector, if needed: its value can be 'cw', 'ccw' or 'neutral'. The last option means 
    the system will automatically choose the optimal alternative.

    e.g. > sidereal=src12,319.256d,70.864d,2000,neutral
    """

def skydip():
    """
    skydip=El1,El2,duration 
    performs an OTF acquisition at the current azimuth position, spanning in
    elevation from El1 to El2 (both expressed in degrees, with 'd' suffix), in duration seconds. 
    A recorded must have previously been enabled in order to save the data.
    """

def startSchedule():
    """
    startSchedule=[project/]schedulename.scd,N 
    runs schedule schedulename.scd (project is the ID of the observing project, it is optional 
    if it has already been input through the projectCode command), reading it from line N
    """

def stopSchedule():
    """
    stopSchedule immediately stops the running schedule, truncating the acquisition
    """

def telescopePark():
    """
    telescopePark parks all the elements: mount (sending the antenna to 
    stow position), minor servo and active surface
    """

def ti():
    """
    ti lists all the active temporized commands
    """

def track():
    """
    track=source
    if the antenna is in ProgramTrack mode and the sourcename is known 
    within the station catalogue (which includes the most commonly 
    observed calibrators), it directly points to the source and tracks it.

    e.g. > track=3c286
    """

def tsys():
    """
    tsys measures the system temperature (K) in the position the antenna is pointing to. 
    It returns a list of values, one for each section in use (e.g. 14 values for the whole MF receiver). 
    All the intermediate steps and calculations are stored in the active logfile
    """

def wait():
    """
    wait=d.d 
    sets a delay (in seconds) which is applied before the system reads/executes the next command
    """

def wx():
    """
    wx returns the current weather parameters: ground temperature (Centigrade), relative humidity (%), 
    atmospheric pressure (hPa), wind speed (km/h).
    """

import copy
myself =  __import__(__name__)
# Create the commands dictionary
commands = myself.__dict__.copy() # Shallow copy, but we do not mind
for cmd in commands.keys():
    if cmd.startswith('__'):
        del commands[cmd]

# Make the setupXXX.__doc__
for cmd in commands:
    if cmd.startswith('setup'):
        setupXXX = getattr(myself, cmd)
        setupXXX.__doc__ = setupCCB.__doc__

