namespace IO;

class ExternalStream : Stream {
instance:
        attribute ioConnection;
        method setConnection(aConnection) {
                ioConnection := aConnection;
        }
        method getConnection() {
                return ioConnection;
        }
        method on(aIOConnection) {
                ioConnection := aIOConnection;
        }
        method close() {
                ioConnection.close();
        }
        method flush() {
                ioConnection.flush();
        }
        method seek(aPosition) {
            ioConnection.seek(aPosition);
        }
        method tell() {
            return ioConnection.tell();
        }
class:
        method on(aIOConnection) {
                var str;
                str := self.new();
                return str.on(aIOConnection);
        }
}
