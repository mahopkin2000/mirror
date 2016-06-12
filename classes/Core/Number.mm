namespace Core;

class Number : Object {
instance:
        method coerce(aNumber) {
                self.subclassResponsibility();
        }
        method generality() {
                self.subclassResponsibility();
        }
        method retryCoercing(aSelector,aNumber) {
	        if (self.generality() < aNumber.generality()) {
		        return (aNumber.coerce(self)).perform(aSelector,[aNumber]);
                } elsif (self.generality() > aNumber.generality()) {
                        var args := [self.coerce(aNumber)];
		        return self.perform(aSelector,args);
                } else {
                        self.error("Coercion attempt failed.");
                }
        }
        
        // generic comparison methods
        method <= aNumber {
                return (self > aNumber).not();
        }
        method > aNumber {
                return aNumber < self;
        }
        method >= aNumber {
                return (self < aNumber).not();
        }        
        method between(aStart,aEnd) {
            return (self >= aStart) && (self <= aEnd);
        }
        
        // Integer methods
        method sumFromInteger(aInteger) {
                return aInteger.retryCoercing("+",self);
        }
        method differenceFromInteger(aInteger) {
                return aInteger.retryCoercing("-",self);
        }
        method productFromInteger(aInteger) {
                return aInteger.retryCoercing("*",self);
        }
        method quotientFromInteger(aInteger) {
                return aInteger.retryCoercing("/",self);
        }
        method lessFromInteger(aInteger) {
                return aInteger.retryCoercing("<",self);
        }
        
        // Float methods
        method sumFromFloat(aFloat) {
                return aFloat.retryCoercing("+",self);
        }
        method differenceFromFloat(aFloat) {
                return aFloat.retryCoercing("-",self);
        }
        method productFromFloat(aFloat) {
                return aFloat.retryCoercing("*",self);
        }
        method quotientFromFloat(aFloat) {
                return aFloat.retryCoercing("/",self);
        }
        method lessFromFloat(aFloat) {
                return aFloat.retryCoercing("<",self);
        }
        
        // arithmetic methods
        method negated() {
                return 0 - self;
        }
        method isNegative() {
                return self < 0;
        }

        // flow control methods
        method timesRepeat(aBlock) {
            for (var i:=0;i<self;i:=i+1) {
					aBlock.value([]);
				}
		  } 
}                       
