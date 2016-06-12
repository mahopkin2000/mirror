import Core::Object;
import Core::Exception;

class OracleException : Exception {
instance:
	attribute errorCode;
	method setErrorCode(aErrorCode) {
		errorCode := aErrorCode;
	}
	method getErrorCode() {
		return errorCode;
	}
class:
	method raise(aErrorCode,aErrorString) {
		var ex := self.new();
		ex.setErrorCode(aErrorCode);
		ex.setMessage(aErrorString);
		ex.raise();
	}
}
