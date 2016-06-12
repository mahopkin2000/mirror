namespace Core;

class UndefinedObject : Object {
instance:
        method isNil() {
                return true;
        }
        method printString() {
                return "nil";
        }
	method isAtomic() {
		return true;
	}
class:
        method new() {
                self.error("Cannot create instances of UndefinedObject");
        }
}
