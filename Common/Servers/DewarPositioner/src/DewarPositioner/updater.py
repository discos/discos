import time 
from IRAPy import logger


class Updater(object):

    def __init__(self, device, control):
        self.device = device
        self.control = control
        self.maxLimit = self.device.getMaxLimit()
        self.minLimit = self.device.getMinLimit()
        self.step = self.device.getStep()

    def updatePosition(self, posgen, vargs):
        try:
            self.control.updating = True

            if not self.device.isReady():
                logger.logError("%s not ready to move: a setup() is required" 
                        %self.device._get_name())
                return
 
            for position in posgen(*vargs):
                if self.control.stop:
                    break
                target = (self.control.starting_position + 
                          self.control.offset + 
                          self.control.rewinding_offset + 
                          position)
                if self.minLimit < target < self.maxLimit:
                    try:
                        self.device.setPosition(target)
                        time.sleep(0.1) # TODO: from CDB
                    except (DerotatorErrors.PositioningErrorEx, DerotatorErrors.CommunicationErrorEx), ex:
                        raeson = "cannot set the %s position" %self.device._get_name()
                        logger.logError('%s: %s' %(raeson, ex.message))
                        break
                    except Exception, ex:
                        raeson = "unknown exception setting the %s position" %self.device._get_name()
                        logger.logError('%s: %s' %(raeson, ex.message))
                        break
                else:
                    logger.logInfo("position %.2f out of range {%.2f, %.2f}: starting rewinding" 
                            %(self.minLimit, self.maxLimit))
                    self.rewind() # blocking call
            self.control.must_update = False
        except KeyboardInterrupt:
            logger.logInfo('stopping Updater.updatePosition() due to KeyboardInterrupt')
        except AttributeError, ex:
            logger.logError('Updater.updatePosition(): attribute error')
            logger.logDebug('Updater.updatePosition(): %s' %ex)
        except Exception, ex:
            logger.logError('unexcpected exception in Updater.updatePosition(): %s' %ex)
        finally:
            self.control.updating = False

    def rewind(self):
        return
        if self.control.modes['rewinding'] == 'AUTO':
            sign = -1 if target < self.maxLimit else +1
            n = (self.maxLimit - self.minLimit) // self.step
            self.control.rewinding_offset += sign * n * step
            # set the isRewinding flag to True
            # set the device position and wait utill it reach the pos
            # set the isRewinding flag to False
        elif self.control.modes['rewinding'] == 'MANUAL':
            pass # Stop the updating and set the flag rewindRequired to True
        else:
            pass # TODO: ERROR!



