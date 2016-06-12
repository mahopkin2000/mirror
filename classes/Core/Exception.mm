namespace Core;

import Collection::Dictionary;

class Exception : Object {
instance:
    attribute message,resumeContext;
    method setMessage(aString) {
        message := aString;
    }
    method getMessage() {
        return message;
    }
    method printString() {
        return "Exception(" + message + ")";
    }
    method printStackTrace(aStream) {
        var ec := thisContext;
        var block,m;
        var sa,se,ip;
        var idx;
        var mn,sf;

        while (ec!=nil) {
            block := ec.getBlock();
            sa := block.getSourceArray();
            m := block.getParent();
            mn:=m.getName();
            sf := m.getSourceFile();
            idx := ec.getIP();
            while (idx>=0) {
                se := sa.at(idx);
                if (se!=nil) {
                        break;
                }
                idx:=idx-1;
            }
            aStream.nextPutAll("\t"+ec.getReceiver().getClass() + "::" 
                + mn + "(" + sf + ":" 
                + se.at(1) + ")").crlf();
            ec := ec.getParent();
        }
    }
    method setResumeContext(aContext) {
        resumeContext := aContext;
    }
    method raise() { 
        var ctxt,rcvr,meth;
        var hd;
        var keys;
        ctxt := Processor.getActiveProcess().getContext().getParent();
        var hb;
        
        while (ctxt != nil) {
            rcvr := ctxt.getReceiver();
            if (self.isKindOf(rcvr)) {
                hd := rcvr.getHandlers();
				if (hd != nil) {
                hb := hd.at(ctxt);
                if (hb!=nil) {
                        hb.value([self]);
                        hd.remove(ctxt);
                        thisContext.setParent(ctxt);
                        return self;
                }
				}
            }
            ctxt := ctxt.getParent();
        } 
        
        // unwound to the top of the call stack, unhandled exception
        OS.stderr().nextPutAll(self.printString() + "\n");
        self.printStackTrace(OS.stderr());
        if (Processor.getActiveProcess()=Processor.getProcessListHead()) {
                OS.exit(-1);
        } else {
                Processor.getActiveProcess().terminate();
        }       
    }
    method resume() {
        thisContext.setParent(resumeContext);
    }
class:
    attribute handlers;
    method getHandlers() {
        return handlers;
    }
    method raise(aString) {
        var ex := self.new();
        ex.setMessage(aString);
        ex.setResumeContext(thisContext.getParent());
        ex.raise();
    }
    method initialize() {
        handlers := Dictionary.new();
    }
    method protect(aActionBlock,aHandlerBlock) {
        handlers.put(thisContext,aHandlerBlock);
        aActionBlock.value([]);    
    }
    method protectWithFinally(aActionBlock,aHandlerBlock,aFinallyBlock) {
    }
}
