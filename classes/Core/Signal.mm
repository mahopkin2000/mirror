namespace Core;

class Signal : Object {
instance:
    attribute signalNumber;
    method on(aSignalNumber) {
        signalNumber := aSignalNumber;
    }
    method getName() {
        var assoc;
        assoc := signalConstants.detect(
            { a | 
                a.getValue()=signalNumber; 
            }
        );
        return assoc.getKey();
    }
    method printString() {
        return self.getName();
    }
class:
    attribute handlers,signalConstants;
    method signalReceived(aSignalNumber) {
        if (handlers.contains(aSignalNumber)) {
            var handler,sig;
            handler := handlers.at(aSignalNumber);
            sig := self.new().on(aSignalNumber);
            handler.value([sig]);
        }
    }
    method registerHandler(aSignalNumber,aHandlerBlock) {
        if (handlers.isNil()) {
            handlers := Dictionary.new();
        }
        handlers.put(aSignalNumber,aHandlerBlock); 
        self.primRegisterHandler(aSignalNumber);
    }
    method unregisterHandler(aSignalNumber,aHandlerBlock) {
        if (handlers.isNil()) {
            handlers := Dictionary.new();
        }
        handlers.remove(aSignalNumber); 
        self.primUnregisterHandler(aSignalNumber);
    }
    primitive method primRegisterHandler(aSignalNumber) {
        self.primitiveFailed();
    }
    primitive method primUnregisterHandler(aSignalNumber) {
        self.primitiveFailed();
    }
    method getSignalConstants() {
        if (signalConstants.isNil()) {
            signalConstants := self.loadSignalConstants();
        }
        return signalConstants;
    }
    primitive method loadSignalConstants() {
        self.primitiveFailed();
    }
    method doesNotUnderstand(aSelector,aArgArray) {
        if ( (aSelector.substring(0,2))="SIG") {
            return self.getSignalConstants().at(aSelector);
        } else {
            return super.doesNotUnderstand(aSelector,aArgArray);
        }
    }
    primitive method raise(aSignalNumber) {
        self.primitiveFailed();
    }
}
