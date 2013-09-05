# Who                                      when            What                                             
# Marco Buttu (mbuttu@oa-cagliari.inaf.it) 28/05/2013      Creation                                  

# Doctrings from the "Observing at the SRT with Nuraghe, Issue n.4, 23/05/13,
# by Simona Righini and Andrea Orlati"

def antennaAzEl():
    """
    antennaAzEl sets the antenna to PRESET mode (fixed Az-El pointings). 
    To enable tracking again, e.g. to run schedules, an antennaTrack 
    or antennaSetup must follow
    """

def antennaPark():
    """
    antennaPark sends the antenna to stow position
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
    antennaStop stops the antenna. Activities can start again only 
    commanding a mode change as antennaTrack (which does not affect the 
    overall setup) or a new setup
    """

def antennaTrack():
    """
    antennaTrack sets the antenna to PROGRAMTRACK mode. It does not change
    the pointing model or any receiver setup
    """

def azelOffsets():
    """
    azelOffsets=double'd',double'd' 
    sets the Az-El offsets (degrees). They are intended "on sky"
    Example: azelOffsets=-0.05d,0.05d
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

def crossScan():
    """
    crossScan=scanFrame,span,duration 
    performs a cross-scan on the previously selected target*, along the 
    scanFrame ('eq','hor' or 'gal'), spanning span degrees in duration seconds. 
    * indicated using the track or sidereal commands
    """

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
    myself =  __import__(__name__)
    cmd = getattr(myself, command)
    return cmd.__doc__

def initialize():
    """
    initialize=code 
    (CCB, KKG, etc...) configures the backend using the default parameters relative to the
    selected receiver. It does NOT act on the receiver, pointing model or antenna mount mode
    """

def lonlatOffsets():
    """
    lonlatOffsets=double'd',double'd' 
    sets the Galactic b-l offsets (degrees). They are intended "on sky".
    Example: lonlatOffsets=2.0d,-1.0d
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

def projectCode():
    """
    projectCode=code 
    lets the system know which project is observing (the code must correspond to the one
    provided by the TAC)
    """

def radecOffsets():
    """
    radecOffsets=double'd',double'd' 
    sets the RA-Dec offsets (degrees). They are intended "on sky".
    Example: radecOffsets=1.0d,0.0d
    """

def receiversSetup():
    """
    receiversSetup=code 
    (CCB, KKG, etc...) configures the receiver using the default parameters. It does
    NOT act on the backend, pointing model or antenna mount mode
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

def setupCCB():
    """
    setupCCB (setupKKG, etc...) unstows the antenna, sets it to tracking mode, 
    selects the pointing model, and configures the receiver and the backend using default parameters. 
    In practice, it is a shortcut corresponding to this sequence: 
    antennaSetup=Code, receiverSetup=receiverCode, initialize=receiverCode, device=0, calOff
    """

def sidereal():
    """
    sidereal=sourcename,RA,Dec,epoch,sector 
    points to the supplied RA-Dec position and temporarily assigns the sourcename label to it. 
    Epoch can be '1950', '2000' or '-1', the last one meaning that the provided coordinates 
    are precessed to the observing epoch. The sector keyword forces the cable wrap
    sector, if needed: its value can be 'cw', 'ccw' or 'neutral'. The last option means 
    the system will automatically choose the optimal alternative.
    Example: sidereal=src12,319.256d,70.864d,2000,neutral
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

def ti():
    """
    ti lists all the active temporized commands
    """

def track():
    """
    track=sourcename 
    points the antenna, in sidereal tracking, to the specified source, which must be
    present in the local catalogue. If you need to insert frequently observed sources 
    in this catalogue, contact the system manager
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

