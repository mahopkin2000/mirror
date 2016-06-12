namespace Core;

class Character : Object {
instance:
        primitive method asInteger() {
                self.primitiveFailed();
        }
        method printString() {
                var s := " ";
                s.put(0,self);
                return s;
        }
	method isAtomic() {
		return true;
	}
	primitive method == aCharacter {
		self.primitiveFailed();
	}
        method isWhiteSpace() {
                return 
                        (self==self.getClass().lf()) ||
                        (self==self.getClass().cr()) ||
                        (self==self.getClass().tab()) ||
                        (self==self.getClass().space());
        }                        
                        
                

class:
        primitive method value(aInteger) {
                self.primitiveFailed();
        }
        method lf() {
                return self.value(10);
        }
        method cr() {
                return self.value(13);
        }
        method tab() {
                return self.value(9);
        }
        method space() {
                return self.value(32);
        }
}
       
