namespace Core;

class Processor : Object {
class:
        attribute processListHead,processListEnd,activeProcess;
        
        method getProcessListHead() {
                return processListHead;
        }
        method getProcessListEnd() {
                return processListEnd;
        }
        method getActiveProcess() {
                return activeProcess;
        }
        method new() {
            self.shouldNotImplement();
        }
}        
