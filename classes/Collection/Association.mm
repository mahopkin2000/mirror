namespace Collection;

import Core::Object;

class Association : Object {
instance:
        attribute key,value;
        
        method setKey(aKey) {
                key := aKey;
        }
        method getKey() {
                return key;
        }
        method setValue(aValue) {
                value := aValue;
        }
        method getValue() {
                return value;
        }
class:
        method keyValue(aKey,aValue) {
                var a := self.new();
                a.setKey(aKey);
                a.setValue(aValue);
                return a;
        }
}
