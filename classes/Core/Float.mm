namespace Core;

class Float : Number {
instance:
        primitive method + aFloat {
                return aFloat.sumFromFloat(self);
        }
        
        primitive method - aFloat {
                return aFloat.differenceFromFloat(self);
        }
        
        primitive method * aFloat {
                return aFloat.productFromFloat(self);
        }
        
        primitive method / aFloat {
                return aFloat.quotientFromFloat(self);
        }
        
        primitive method < aFloat {
                return aFloat.lessFromFloat(self);
        }
        
        primitive method > aFloat {
                return super > aFloat;
        }
        
        primitive method = aFloat {
                return aFloat.equalFromFloat(self);
        }
        primitive method printString() {
                self.primitiveFailed();
        }
        method asFloat() {
                return self;
        }
        method coerce(aNumber) {
                return aNumber.asFloat();
        }
        method generality() {
                return 100;
        }
        method isAtomic() {
            return true;
        }
		  primitive method asInteger() {
                self.primitiveFailed();
        }
class:
        primitive method fromValue(aNumber) {
            self.primitiveFailed();
        }
}
