namespace Net;

import Core::Object;
import Core::OS;
import IO::ExternalReadStream;
import IO::ExternalWriteStream;
import Core::Exception;

class Socket : Object {
instance:
        attribute socketHandle;
        
        method open(aType,aFamily) {
                socketHandle := self.primOpen(aType,aFamily);
                return socketHandle;
        }
        primitive method primOpen(aType,aFamily) {
                self.primitiveFailed();
        }
        method getSocketHandle() {
                return socketHandle;
        }
        method setSocketHandle(aHandle) {
                socketHandle := aHandle;
        }
        method close() {
                return self.primClose(socketHandle);
        }
        primitive method primClose(aSocketHandle) {
                self.primitiveFailed();
        }
        method bind(aSocketAddress) {
                return self.primBind(   socketHandle,
                                        aSocketAddress.getAddress(),
                                        aSocketAddress.getPort());
        }
        primitive method primBind(aSocketHandle,aAddress,aPort) {
                self.primitiveFailed();
        }
        method connect(aSocketAddress) {
                return self.primConnect( socketHandle,
                                        aSocketAddress.getAddress(),
                                        aSocketAddress.getPort());
        }
        primitive method primConnect(aSocketHandle,aAddress,aPort) {
            Exception.raise(OS.strerror(OS.errno()));
            self.primitiveFailed();
        }
        method listen(aBacklog) {
                return self.primListen(socketHandle,aBacklog);
        }
        primitive method primListen(aSocketHandle,aBacklog) { 
                self.primitiveFailed();
        }
        method accept() {
                var fd;
                var sa := SocketAddress.new();
                fd := self.primAccept(socketHandle,sa);
                if (fd < 0) {
                        self.error("failure in accept call with code " + fd);
                } else {
                        var ns := Socket.new();
                        ns.setSocketHandle(fd);
                        return ns;
                }
        }
        primitive method primAccept(aSocketHandle,aSocketAddress) {
                self.primitiveFailed();
        }
        method readInto(aByteArray,aStartPos,aNumBytes) {
                return self.primReadInto(socketHandle,aByteArray,aStartPos,aNumBytes);
        }
        primitive method primReadInto(aHandle,aByteArray,aStartPos,aNumBytes) {
                self.primitiveFailed();
        }
        method writeChar(aCharacter) {
                self.primWriteChar(socketHandle,aCharacter);
        }
        primitive method primWriteChar(aHandle,aCharacter) {
                self.error("Primitive failure: " + OS.strerror(OS.errno()));
        }
        method readChar() {
                return self.primReadChar(socketHandle);
        }
        primitive method primReadChar(aHandle) {
                self.primitiveFailed();
        }
        method writeFrom(aString,aStartPos,aNumBytes) {
                self.primWriteFrom(socketHandle,aString,aStartPos,aNumBytes);
        }
        primitive method primWriteFrom(aHandle,aString,aStartPos,aNumBytes) {
                self.primitiveFailed();
        }
        method flush() {
                self.primFlush(socketHandle);
        }
        primitive method primFlush(aHandle) {
                self.primitiveFailed();
        }
        method readStream() {
            return ExternalReadStream.on(self);
        }
        method writeStream() {
            return ExternalWriteStream.on(self);
        }
        method getSocketAddress() {
            var sa := SocketAddress.new();
            self.primGetSockName(socketHandle,sa);
            return sa;
        }
        primitive method primGetSockName(aHandle,aSocketAddress) {
            self.primitiveFailed();
        }
        method getPeerAddress() {
            var sa := SocketAddress.new();
            self.primGetPeerName(socketHandle,sa);
            return sa;
        }
        primitive method primGetPeerName(aHandle,aSocketAddress) {
            self.primitiveFailed();
        }
class:
        primitive method sockStream() {
                self.primitiveFailed();
        }
        primitive method sockDGram() {
                self.primitiveFailed();
        }
        primitive method sockRaw() {
                self.primitiveFailed();
        }
        primitive method sockRDM() {
                self.primitiveFailed();
        }
        primitive method sockSeqPacket() {
                self.primitiveFailed();
        }
        primitive method protoICMP() {
                self.primitiveFailed();
        }
        primitive method protoTCP() {
                self.primitiveFailed();
        }
        primitive method protoUDP() {
                self.primitiveFailed();
        }
        primitive method protoRaw() {
                self.primitiveFailed();
        }
        primitive method htons(aInteger) {
            self.primitiveFailed();
        }
        primitive method htonl(aInteger) {
            self.primitiveFailed();
        }
        primitive method ntohl(aByteArray) {
            self.primitiveFailed();
        }
        primitive method ntohs(aByteArray) {
            self.primitiveFailed();
        }
        primitive method getHostByName(aHostname) {
            self.primitiveFailed();
        }

        method open(aType,aFamily) {
                var s := Socket.new();
                s.open(aType,aFamily);
                return s;
        }
        
        // test harness
        method test(args) {
                // try to connect to a time service
                var sa := SocketAddress.from("161.44.11.19",37);
                var s := Socket.open(self.sockStream(),self.protoTCP());
                var rc := s.connect(sa);
                if (rc < 0) {
                        var en:=OS.errno();
                        OS.stdout().nextPutAll("rc is " + rc + ",errno is " + en).crlf();
                        OS.exit(-1);
                }
                var rs := s.readStream();
                var ch;
                while ((ch:=rs.next())!=nil) {
                        OS.stdout().nextPut(ch);
                }
                s.close();
        }
                
}
