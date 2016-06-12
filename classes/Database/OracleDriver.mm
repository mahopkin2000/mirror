namespace Database;

import Core::Object;

class OracleDriver : Object {
class:
        primitive method connect(aUsername,aPassword,aDatabaseName) {
		self.primitiveFailed();
        }
        primitive method disconnect(aLDAHandle) {
                self.primitiveFailed();
        }
        primitive method rollback(aLDAHandle) {
                self.primitiveFailed();
        }
        primitive method commit(aLDAHandle) {
                self.primitiveFailed();
        }
        primitive method prepare(aLDAHandle,aStatement) {
                self.primitiveFailed();
        }
        primitive method closeCursor(aLDAHandle,aCDAHandle) {
                self.primitiveFailed();
        }
        primitive method bindVariable(aLDAHandle,aCDAHandle,aPosition,aValue) {
                self.primitiveFailed();
        }
        primitive method execute(aLDAHandle,aCDAHandle,aReturnArray) {
                self.primitiveFailed();
        }
        primitive method fetch(aLDAHandle,aCDAHandle,aColBindings) {
                self.primitiveFailed();
        }
}
                 
