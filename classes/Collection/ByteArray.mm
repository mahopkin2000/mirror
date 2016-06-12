namespace Collection;

import Core::Object;
import Core::Character;

byte class ByteArray : Object {
instance:
	primitive method at(anIndex) {
		self.primitiveFailed();
	}
	primitive method put(anIndex,aByte) {
		self.primitiveFailed();
	}
	primitive method size() {
		self.primitiveFailed();
	}
    method isAtomic() {
        return true;
    }
    method asString() {
        var size := self.size();
        var s:= String.indexedNew(size);
        for (var i:=0;i<size;i:=i+1) {
            s.put(i,Character.value(self.at(i))); 
        }
        return s;
    }
}
