namespace Core;

class RegExp : Object {
instance:
    attribute compiledHandle;
    primitive method match(aString) {
        self.primitiveFailed();
    }
    primitive method free() {
        self.primitiveFailed();
    }
    method release() {
        OS.stdout().nextPutAll("In release method for regexp\n");
        if (compiledHandle!=nil) {
            OS.stdout().nextPutAll("Freeing regexp\n");
            self.free();
        }
    }
    primitive method compile(aPattern,aFlagList) {
        self.primitiveFailed();
    }
class:
    method compile(aPattern,aFlagList) {
		var re := self.new();
		re.compile(aPattern,aFlagList);
		return re;
    }
}
