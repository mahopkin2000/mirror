namespace Core;

class Behavior : Object {
instance:
        attribute methodDictionary,superclass,format;
        primitive method new() {
                self.primitiveFailed();
        }
        primitive method new() {
                self.primitiveFailed();
        }
        primitive method indexedNew(slots) {
                self.primitiveFailed();
        }   
        method methodDictionary() {
                return methodDictionary;
        }
        method superclass() {
                return superclass;
        }
        method format() {
                return format;
        } 
        method respondsTo(aSelector) {  
                //OS.stdout().nextPutAll("in respondsTo with selector " + aSelector).crlf();
                if (methodDictionary.contains(aSelector)) {
                        return true;
                } elsif (superclass==nil) {
                        return false;
                } else {
                        return superclass.respondsTo(aSelector);
                }
        }
        method isKindOf(aClass) {
                if (self==aClass) {
                        return true;
                } elsif (superclass==nil) {
                        return false;
                } else {
                        return superclass.isKindOf(aClass);
                }
        }
}

