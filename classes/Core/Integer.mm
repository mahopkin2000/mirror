namespace Core;

class Integer : Number {
instance:
        primitive method + aInteger {
                return aInteger.sumFromInteger(self);
        }
        
        primitive method - aInteger {
                return aInteger.differenceFromInteger(self);
        }
        
        primitive method * aInteger {
                return aInteger.productFromInteger(self);
        }
        
        primitive method / aInteger {
                return aInteger.quotientFromInteger(self);
        }
        
        primitive method % aInteger {
                self.primitiveFailed();
        }
        
        primitive method < aInteger {
                return aInteger.lessFromInteger(self);
        }
        
        primitive method > aInteger {
                return super > aInteger;
        }
        primitive method == aInteger {
                self.primitiveFailed();
        }        
        primitive method & aInteger {
                self.primitiveFailed();
        }        
        primitive method | aInteger {
                self.primitiveFailed();
        }        
        primitive method ^ aInteger {
                self.primitiveFailed();
        }        
        primitive method << aInteger {
                self.primitiveFailed();
        }        
        primitive method >> aInteger {
                self.primitiveFailed();
        }        
        primitive method onesComplement() {
                self.primitiveFailed();
        }        
        primitive method printString() {
                self.primitiveFailed();
        }
        method @ aInteger {
                return Point.fromCoordinates(self,aInteger);
        }
        method asFloat() {
                return Float.fromValue(self);
        }
        method asInteger() {
                return self;
        }
        method coerce(aNumber) {
                return aNumber.asInteger();
        }
        method generality() {
                return 50;
        }
        method isAtomic() {
            return true;
        }
class:
}
