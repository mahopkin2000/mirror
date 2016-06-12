namespace Core;

byte class LargeNumber : Number {
instance:
        primitive method + aNumber {
                return aNumber.sumFromInteger(self);
        }
        
        primitive method - aNumber {
                return aNumber.differenceFromInteger(self);
        }
        
        primitive method * aNumber {
                return aNumber.productFromInteger(self);
        }
        
        primitive method / aNumber {
                return aNumber.quotientFromInteger(self);
        }
        
        primitive method < aNumber {
                return aNumber.lessFromInteger(self);
        }
        
        primitive method > aNumber {
                return super > aNumber;
        }
        
        primitive method = aNumber {
                self.primitiveFailed();
        }        

        primitive method % aNumber {
                self.primitiveFailed();
        }        
        
        method isAtomic() {
            return true;
        }

        method coerce(aNumber) {
                return self.getClass().fromValue(aNumber);
        }

        method generality() {
            return 150;
        }
class:
        primitive method fromValue(aValue) {
            self.primitiveFailed();
        } 
        primitive method fromString(aString) {
            self.primitiveFailed();
        }
}
