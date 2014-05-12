from multiprocessing import Process, Value

class Positioner(object):
    def __new__(cls, derotator):
        """Singleton."""
        if not hasattr(cls, '_instance'):
            cls._instance = super(Positioner, cls).__new__(cls, derotator)
        return cls._instance # TODO: or better, raise an exception

    def __init__(self, antenna, derotator):
        self.derotator = derotator
        self.create()

    def create(self):
        self._update = Value('I', 1)
        self._on_source = Value('I', 1)
        self._terminate = Value('I', 0)
        args = (self.conf, self.positions, self._update, self._on_source, self._terminate)
        self.p = Process(target=self.updatePosition, args=args)
        self.daemon = True

    def start(self):
        self.p.start()

    def terminate(self, delay=1):
        self._terminate.value = True 
        time.sleep(delay)
        self.p.terminate()
        self.p.join()

    def terminated(self):
        return self._terminate.value

    @staticmethod
    def updatePosition(conf, positions, update, on_source, terminate):
        """Compute and set the target position.

        Arguments
        =========
        - update: if 1 the process updates the position, elsewhere it does not.
        - on_source: if 1 then the target is the source, if 0 it is the off source position
        - terminate: if 1 the process terminates properly

        The position changes from on source to off source when `on_source` flag changes from
        1 to 0, and viceversa from the off source position to on source one.
        """
        if not conf.simulate:
            try:
                from devices import antenna, ANT_J2000, ACU_NEUTRAL
            except Exception, e:
                logging.exception('Cannot get the antenna component')
                print 'ERROR: Cannot get the antenna component'
                terminate.value = True
                sys.exit(1)

        try:
            while True:
                if update.value:
                    # Get the position from the positions list
                    at_time = datetime.datetime.utcnow()
                    found = False 
                    size = len(positions)
                    idx = 0
                    for idx, item in enumerate(positions[idx:]):
                        date, ra, dec = item
                        if date == at_time:
                            found = True
                            break
                        if date < at_time:
                            if idx == size - 1:
                                found = True
                                break # The last element
                            format_ = '%Y-%b-%d %H:%M'
                            t0, t1 = date, positions[idx+1][0]
                            ra0, ra1 = ra, positions[idx+1][1]
                            dec0, dec1 = dec, positions[idx+1][2]
                            if t0 < at_time < t1:
                                ra = ra0 + (ra1 - ra0)*((at_time - t0).seconds/(t1 - t0).seconds)
                                dec = dec0 + (dec1 - dec0)*((at_time - t0).seconds/(t1 - t0).seconds)
                                found = True
                                break 
                            else:
                                continue
                    if found:
                        ra_offset = math.radians(conf.offset) # Offset in radians
                        ra = ra if on_source.value else ra + ra_offset
                        ra_py, dec_py = conf.target.getRaDec() if on_source.value else \
                                map(lambda x, y: x + y, conf.target.getRaDec(), (ra_offset, 0))
                        # if abs(ra - ra_py) > 2e-4:
                        #     logging.warning('Mismatch between horizons and pyephem RAs: (%.4f, %.4f)', ra, ra_py)
                        #     print '@ WARNING: Mismatch between horizons and pyephem RAs: (%.4f, %.4f)' %(ra, ra_py)
                        # if abs(dec - dec_py) > 2e-4:
                        #     logging.warning('@ Mismatch between horizons and pyephem DECs: (%.4f, %.4f)', dec, dec_py)
                        #     print '@ WARNING: Mismatch between horizons and pyephem DECs: (%.4f, %.4f)' %(dec, dec_py)
                        del positions[:idx]
                    else:
                        raise Exception('Cannot get a proper date in the horizons file')

                    # If it is not a simulation, set the antenna position
                    if not conf.simulate:
                        try:
                            # antenna.sidereal(conf.target.name, ra, dec, ANT_J2000, ACU_NEUTRAL)
                            antenna.sidereal(conf.target.name, ra_py, dec_py, ANT_J2000, ACU_NEUTRAL)
                        except Exception, e:
                            logging.exception('Cannot set the antenna position')
                            print 'ERROR: Cannot set the antenna position'
                            terminate.value = True
                            sys.exit(1)

                    where = 'on source' if on_source.value else 'off source'
                    now = datetime.datetime.utcnow()
                    logging.info('Tracking %s: ra_dec(%.4f, %.4f) @ %s', where, ra, dec, now)
                if terminate.value:
                    break
                time.sleep(conf.positioning_time)
        except KeyboardInterrupt:
            print 'Stopping the positioning...'
        except Exception, e:
            print 'UNEXPECTED ERROR in updatePosition(): \n\t%s' %e
            logging.exception('Got exception on updatePoition()')
        finally:
            if terminate.value:
                sys.exit(0)



