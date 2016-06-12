namespace Core;

import Collection::Dictionary;

class Object {
instance:
        primitive method basicAt(aIndex) {                
                self.primitiveFailed();
        }
        primitive method basicPut(aIndex,aObject) {        
                self.primitiveFailed();
        }
        method at(aIndex) {
                if (aIndex.between(0,self.size()-1).not()) {
                        self.error("Index " + aIndex + " out of bounds");
                }
                return self.basicAt(aIndex);
        }
        method put(aIndex,aObject) {
                if (aIndex.between(0,self.size()-1).not()) {
                        self.error("Index " + aIndex + " out of bounds");
                }
                self.basicPut(aIndex,aObject);
        }
        primitive method copy() {
                self.primitiveFailed();
        }
        method size() {
                return self.basicSize();
        }
        primitive method basicSize() {
                self.primitiveFailed();
        }
        primitive method getClass() {
                self.primitiveFailed();
        } 
        primitive method hash() {
                self.primitiveFailed();
        }
        primitive method == aObject {
                self.primitiveFailed();
        }
        method = aObject {
                return self==aObject;
        }
        method != aObject {
                return (self==aObject).not();
        }
        primitive method become(aObject) {
                self.primitiveFailed();
        }
        method error(aString) {
            Exception.raise(aString);
        }
        primitive method perform(aSelector,aArgArray) {
                self.primitiveFailed();
        }
        method subclassResponsibility() {
                self.error("This method should have been overridden by my subclass.");
        }
        method initialize() {
        }
        method yourself() {
                return self;
        }
        method printString() {
                return "a " + self.getClass().getName();
        }
        method isNil() {
                return false;
        }
        method primitiveFailed() {
                self.error("A primitive has failed.");
        }
        method shouldNotImplement() {
                self.error("This method is not appropriate for this object.");
        }
        method doesNotUnderstand(selector,args) {
                self.error("Message not understood: " + selector);
        } 
        method allInstanceVariables() {
                return self.getClass().allInstanceVariables();
        }
        method getInstanceVariableIndex(aInstVarName) {
                return self.getClass().getInstanceVariableIndex(aInstVarName);                
        }
        method respondsTo(aSelector) {
                return self.getClass().respondsTo(aSelector);
        } 
        method isKindOf(aClass) {
                //OS.stdout().nextPutAll("self.getClass() is " + self.getClass()).crlf();
                return self.getClass().isKindOf(aClass);
        }
        method isMemberOf(aClass) {
                return self.getClass()==aClass;
        }
        method isInstanceOf(aClass) {
                return self.getClass()==aClass;
        }
	method deepCopy() {
		return self.deepCopyImpl(Dictionary.new());
	}
	method deepCopyImpl(aScoreCard) {
		var c,c1;
		if (self.isAtomic()) {
			return self;
        } elsif (aScoreCard.contains(self)) {
            //OS.stdout().nextPutAll("Already copied object: " + 
            //    aScoreCard.at(self) + ",returning reference").crlf();
            return aScoreCard.at(self);
		} else {
			c:= self.copy();
            //OS.stdout().nextPutAll("Made copy - " + c.getPointer() +
             //   ":" + c).crlf();
            aScoreCard.put(self,c);
			for (var i:=0;i<self.size();i:=i+1) {
				//OS.stdout().nextPutAll(self.printString() + ":" + self.getPointer() +
                 //   ":slot " + i + ":deepCopying object " + self.basicAt(i)).crlf();
                c1 := self.basicAt(i).deepCopyImpl(aScoreCard);
				c.basicPut(i,c1);
			}	
		}
		return c;
	}
	method isAtomic() {
		return false;
	}
    primitive method getPointer() {
        self.primitiveFailed();
    }
    primitive method refcnt() {
        self.primitiveFailed();
    }
    primitive method getFlags() {
        self.primitiveFailed();
    }
    primitive method setFlags(flags) {
        self.primitiveFailed();
    }
class:
    method new() {
        var no := self.basicNew();
        no.initialize();
        return no;
    }
    primitive method basicNew() {
        self.primitiveFailed();
    }
    primitive method indexedNew(aInteger) {
        self.primitiveFailed();
    }
}

