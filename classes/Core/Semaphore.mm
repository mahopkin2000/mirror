namespace Core;

import Collection::LinkedList;

class Semaphore : Object {
instance:
    attribute excessSignals,processList;
    method initialize() {
        excessSignals := 0;
        processList := LinkedList.new();
    }
    primitive method wait() {
        self.primitiveFailed();
    }
    primitive method signal() {
        self.primitiveFailed();
    }
    method critical(aBlock) {
        self.wait();
        var v := aBlock.value([]);
        self.signal();
        return v;
    }
    method bumpExcessSignals() {
        excessSignals := excessSignals + 1;
    }
class:
    method forMutualExclusion() {
        var s := self.new();
        s.bumpExcessSignals();
        return s;
    }
}        
