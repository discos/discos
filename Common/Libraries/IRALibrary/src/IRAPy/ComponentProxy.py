#!/usr/bin/env python
from threading import Lock
from atexit import register, unregister
from maciErrType import CannotGetComponentEx
from omniORB.CORBA import COMM_FAILURE
from ACSErrTypeCommonImpl import CORBAProblemExImpl
from Acspy.Common.CDBAccess import CDBaccess
from Acspy.Util.ACSCorba import getManager
from Acspy.Clients.BaseClient import BaseClient
from Acspy.Servants.ContainerServices import ContainerServices
from Acspy.Common.Log import getLogger, startPeriodicFlush
from ClientErrorsImpl import DISCOSOfflineErrorExImpl


class ComponentProxy:

    def __init__(self, component_name=None, component_idl=None):
        if (not component_name and not component_idl) or \
                (component_name and component_idl):
            raise ValueError(
                "Provide one between 'component_name' and 'component_idl'"
            )
        self._client = AsyncACSClient()
        self._component_ref = None
        self._component_name = component_name
        self._component_idl = component_idl
        try:
            # Try to connect immediately
            self._ensure_connection()
        except:
            pass

    def __dir__(self):
        attrs = set(super().__dir__())
        if self._component_ref:
            attrs = set(dir(self._component_ref)).union(attrs)
        return sorted(attrs)

    def __getattr__(self, attr):
        self._ensure_connection()
        return getattr(self._component_ref, attr)

    @staticmethod
    def _isManagerOnline():
        try:
            getManager().ping()
            return True
        except:
            return False

    def _ensure_connection(self):
        if not self._isManagerOnline():
            raise DISCOSOfflineErrorExImpl()
        if not self._client.isLoggedIn():
            self._client.login()
        if self._component_ref is not None:
            try:
                self._component_ref._non_existent()
                return
            except COMM_FAILURE:
                self._component_ref = None
        try:
            if self._component_name:
                self._component_ref = self._client.getComponent(
                    self._component_name
                )
            elif self._component_idl:
                self._component_ref = self._client.getDefaultComponent(
                    self._component_idl
                )
                self._component_name = self._component_ref.name
        except CannotGetComponentEx:
            raise DISCOSOfflineErrorExImpl()


class AsyncContainerServices(ContainerServices):

    def __init__(self):
        self._ContainerServices__name = None
        self._ContainerServices__contname = None
        self._ContainerServices__token = None
        self._ContainerServices__handle = None
        self._ContainerServices__logger = None
        self.__inner_cdb_access = None
        self.activateOffShootMethod = None

    @property
    def _ContainerServices__cdb_access(self):
        if self.__inner_cdb_access is None:
            self.__inner_cdb_access = CDBaccess()
        return self.__inner_cdb_access


class AsyncBaseClient(BaseClient):

    def __init__(self, name="Python Client"):
        self.token = None
        self.name = str(name)
        self.managerComponents = []
        self.corbaRef = None
        self.loggedIn = False
        self.loggingIn = Lock()


class AsyncACSClient(AsyncBaseClient, AsyncContainerServices):

    def __init__(self, name="Python Client"):
        AsyncBaseClient.__init__(self, name)
        AsyncContainerServices.__init__(self)
        register(self.logout)

    def login(self):
        self.token = self.managerLogin()
        try:
            self.logger
        except:
            self.logger = getLogger(self.name)
        startPeriodicFlush(3)
        self.setAll(
            self.name,
            self.token,
            self.token.h,
            self.__activateOffshoot
        )

    def logout(self):
        self.managerLogout()
        unregister(self.logout)

    def __activateOffshoot(self, comp_name, py_object):
        try:
            return py_object._this()
        except:
            raise CORBAProblemExImpl()
