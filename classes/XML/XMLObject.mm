namespace XML;

import Core::Object;
import Core::Dictionary;
import Core::OS;
import XML::ExpatParser;

class XMLObject : Object {
instance:
        attribute map;
        method initialize() {
                map := Dictionary.new();
        }
        method setProperty(aKey,aValue) {
                map.put(aKey,aValue);
        }
        method getPropertyValue(aKey) {
                return map.at(aKey);
        }
        method doesNotUnderstand(aSelector,aArgArray) {
                if (aArgArray.size()==0) {
                        return map.at(aSelector);
                } else {
                        map.put(aSelector,aArgArray.at(0));
                }
        }
class:
        method test(args) {
                var xmlobj:=XMLObject.new();
                xmlobj.address("3912 Yateswood Court");
                OS.stdout().nextPutAll("address property is " +
                        xmlobj.address()).crlf();
                
        }
}


