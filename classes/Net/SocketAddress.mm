namespace Net;

import Core::Object;

class SocketAddress : Object {
instance:
        attribute address,port;
        method getAddress() {
                return address;
        }
        method setAddress(aAddress) {
                address := aAddress;
        }
        method getPort() {
                return port;
        }
        method setPort(aPort) {
                port := aPort;
        }
class:
        method from(aAddress,aPort) {
                var sa := self.new();
                sa.setAddress(aAddress);
                sa.setPort(aPort);
                return sa;
        }
}
