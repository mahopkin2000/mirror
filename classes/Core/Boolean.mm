namespace Core;

class Boolean : Object {
instance:
        method || aBoolean {
                self.subclassResponsibility();
        }

        method or(aBlock) {
                self.subclassResponsibility();
        }
        
        method && aBoolean {
                self.subclassResponsibility();
        }

        method and(aBlock) {
                self.subclassResponsibility();
        }
        
        method not() {
                self.subclassResponsibility();
        } 
	method isAtomic() {
		return true;
	}
class:
        method new() {
                self.error("Cannot create instances of Boolean");
        }
}
