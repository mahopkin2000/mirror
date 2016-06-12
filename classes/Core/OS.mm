namespace Core;

import IO::ExternalWriteStream;
import IO::ExternalReadStream;
import IO::FileConnection;

class OS : Object {
class:
        primitive method primStdoutHandle() {
                self.primitiveFailed();
        }
        method stdout() {
                return ExternalWriteStream.on(FileConnection.on(self.primStdoutHandle()));
        }  
        primitive method primStderrHandle() {
                self.primitiveFailed();
        }
        method stderr() {
                return ExternalWriteStream.on(FileConnection.on(self.primStderrHandle()));
        }   
        primitive method primStdinHandle() {
                self.primitiveFailed();
        }
        method stdin() {
                return ExternalReadStream.on(FileConnection.on(self.primStdinHandle()));
        }   
        primitive method exit(aExitCode) {
                self.primitiveFailed();
        }   
        primitive method system(aCommand) {
                self.primitiveFailed();
        }
        primitive method fork() {
                self.primitiveFailed();
        }
        primitive method exec(aProgramName) {
                self.primitiveFailed();
        }
        primitive method getenv(aEnvVarName) {
                self.primitiveFailed();
        }
        primitive method setenv(aEnvVarName,aValue) {
                self.primitiveFailed();
        }
        primitive method errno() {
                self.primitiveFailed();
        }
        primitive method strerror(aErrno) {
                self.primitiveFailed();
        }
        primitive method fork() {
                self.primitiveFailed();
        }
        primitive method exec(aProgram,aArgArray) {
                self.primitiveFailed();
        }
        primitive method sleep(aNumSeconds) {
                self.primitiveFailed();
        }
        primitive method alarm(aNumSeconds) {
                self.primitiveFailed();
        }
        primitive method wait() {
                self.primitiveFailed();
        }
        primitive method waitpid(aPid,aOptionsFlag) {
                self.primitiveFailed();
        }
        primitive method kill(aPid,aSignalNumber) {
                self.primitiveFailed();
        }
        
        // test harness method
        method test(args) {
                OS.stdout().nextPutAll("getenv(JAVA_HOME) is " +
                        OS.getenv("JAVA_HOME")).crlf();
        }
        
                      
}
