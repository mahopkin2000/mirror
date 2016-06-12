namespace IO;

import Core::Object;

class FileConnection : Object {
instance:
        attribute handle;
        method getHandle() {
                return handle;
        }
        method setHandle(aHandle) {
                handle := aHandle;
        } 
        method open(aPathname,aMode) {
                handle := self.primOpen(aPathname,aMode);
        }
        primitive method primOpen(aPathname,aMode) {
                self.primitiveFailed();
        }
        method writeFrom(aString,aStartPos,aNumBytes) {
                return self.primWriteFrom(handle,aString,aStartPos,aNumBytes);
        }
        primitive method primWriteFrom(aHandle,aString,aStartPos,aNumBytes) {
                self.primitiveFailed();
        }
        method readInto(aByteArray,aStartPos,aNumBytes) {
                return self.primReadInto(handle,aByteArray,aStartPos,aNumBytes);
        }
        primitive method primReadInto(aHandle,aByteArray,aStartPos,aNumBytes) {
                self.primitiveFailed();
        }
        method writeChar(aCharacter) {
                self.primWriteChar(handle,aCharacter);
        }
        primitive method primWriteChar(aHandle,aCharacter) {
                self.primitiveFailed();
        }
        method readChar() {
                return self.primReadChar(handle);
        }
        primitive method primReadChar(aHandle) {
                return nil;
        }
        method flush() {
                self.primFlush(handle);
        }
        primitive method primFlush(aHandle) {
                self.primitiveFailed();
        }
        method close() {
                self.primClose(handle);
        }
        primitive method primClose(aHandle) {
                self.primitiveFailed();
        } 
        method seek(aPosition) {
                self.primSeek(handle,aPosition);
        }
        method primSeek(aHandle,aPosition) { 
                self.primitiveFailed();
        }      
        method tell() {
                return self.primTell(handle);
        }
        method primTell(aHandle) { 
                self.primitiveFailed();
        }      
class:
        method open(aPathname,aMode) {
                return self.new().open(aPathname,aMode);
        }
        method on(aHandle) {
                return self.new().setHandle(aHandle);
        }
}             
