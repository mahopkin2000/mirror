namespace Database;

import Core::Object;

class Connection : Object {
instance:
        attribute errorCode,errorString;
        method getErrorCode() {
                return errorCode;
        }
        method getErrorString() {
                return errorString;
        }
        method executeSQL(aStatement) {
                self.subclassResponsibility();
        }
        method prepare(aStatement) {
                self.subclassResponsibility();
        }
        method disconnect() {
                self.subclassResponsibility();
        }
        method rollback() {
                self.subclassResponsibility();
        }
        method commit() {
                self.subclassResponsibility();
        }
class:
}
