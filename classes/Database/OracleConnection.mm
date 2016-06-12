namespace Database;

import Database::Connection;
import Database::OracleDriver;
import Core::OS;
import Database::OracleCursor;

class OracleConnection : Connection {
instance:
        attribute connectionHandle,errorFlag,errorString;
        
        method initialize() {
                connectionHandle := nil;
                errorFlag := false;
                errorString:="";
        }
        method setHandle(aHandle) {
                connectionHandle := aHandle;
        }
        method prepare(aStatement) {
                var cursorHandle;
                cursorHandle := OracleDriver.prepare(connectionHandle,aStatement);
                return OracleCursor.on(connectionHandle,cursorHandle);
        }
        method disconnect() {
                OracleDriver.disconnect(connectionHandle);
        }
        method rollback() {
                OracleDriver.rollback(connectionHandle);
        }
        method commit() {
                OracleDriver.commit(connectionHandle);
        }
class:
        method connect(aUsername,aPassword,aDatabaseName) {
                var connectionHandle;
                connectionHandle := OracleDriver.connect(
                    aUsername,aPassword,aDatabaseName);
                var nc := self.new();
                nc.setHandle(connectionHandle);
                return nc;
        }
}
