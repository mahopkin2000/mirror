namespace Core;

import Collection::Array;
       
class BlockContext : Object {
instance:
    attribute block,parent,stack,ip,sp,receiver,nestedScopes;
    attribute scopeLevel;
    method setupContext(aBlock,aParent,aReceiver,aArgArray) {
        block := aBlock;
        parent := aParent;
        receiver := aReceiver;
        ip := 0;
        sp := -1;
        stack := Array.indexedNew(20);
        var sta := block.getNestedScopesTemplate();
        var sa := Array.indexedNew(sta.size());
        for (var i:=0;i<sta.size();i:=i+1) {
            sa.put(i,Array.indexedNew(sta.at(i).size()-1)); 
        }
        nestedScopes := sa;
        scopeLevel := 0;
    }
    method getBlock() {
        return block;
    }
    method getReceiver() {
        return receiver;
    }
    method getParent() {
        return parent;
    }
    method setParent(aContext) {
        parent := aContext;
    }
    method getIP() {
        return ip;
    }
    method getNestedScopes() {
        return nestedScopes;
    }
    method getScopeLevel() {
        return scopeLevel;
    }
    primitive method value(args) {
        self.primitiveFailed();
    }                
    primitive method newProcess(args) {
        self.primitiveFailed();
    }
    primitive method fork(args) {
        self.primitiveFailed();
    }
    method whileTrue(aBlock) {
        while (self.value([])) {
                aBlock.value([]);
        }
    }
    method whileFalse(aBlock) {
        while (self.value([]).not()) {
                aBlock.value([]);
        }
    }
    method ifTrue(aBlock) {
        if (self.value([])) {
                aBlock.value([]);
        }
    }
    method ifFalse(aBlock) {
        if (self.value([]).not()) {
                aBlock.value([]);
        }
    }

    // simulation stuff
    method top() {
        if (sp>=0) {
            return stack.at(sp);
        } else {
            self.error("Stack underflow error");
        }
    }
    method pop() {
        if (sp>=0) {
            var rv := stack.at(sp);
            stack.put(sp,nil);
            sp:=sp-1;
            //OS.stdout().nextPutAll("popped stack, sp="+sp).crlf();
            return rv;
        } else {
            self.error("Stack underflow error");
        }
    }
    method push(anObject) {
        sp:=sp+1;
        stack.put(sp,anObject);
        //OS.stdout().nextPutAll("pushed stack, sp="+sp).crlf();
    }
    method isEmpty() {
        return (sp<0);
    }
    method interpretNextByte() {
        var bytecode := block.getBytecode();
        var rc;

        if (    (bytecode.size() > 0) &&
                (ip < bytecode.size())  ) {
            var bc := block.getBytecode().at(ip);
            //OS.stdout().nextPutAll("bc is " + bc).crlf();

            if (bc>=100) && (bc<=105) {
                self.processJumpByte(bc);
            } elsif (bc>=112) && (bc<=115) {
                self.processSpecialLiteral(bc);
            } elsif (bc==124) {
                self.processReturn();
                return parent;
            } elsif (bc==132) || (bc==133) {
                rc:=self.processMsgSend(bc);
                if (rc!=nil) {
                    ip:=ip+1;
                    return rc;
                }
            } elsif (bc==135) {
                self.pop();
            } elsif (bc==139) {
                self.processInstVarFetch();
            } elsif (bc==140) {
                self.processLiteralFetch();
            } elsif (bc==141) {
                self.processInstVarStore();
            } elsif (bc==142) {
                self.processArrayLiteral();
            } elsif (bc==143) {
                self.processBlockClosure();
            } elsif (bc==145) {
                self.processLocalVarFetch();
            } elsif (bc==146) {
                self.processLocalVarStore();
            } elsif (bc==147) {
                self.processClassInstVarStore();
            } elsif (bc==148) {
                self.processClassInstVarFetch();
            } elsif (bc==150) {
                self.processScopeLevel();
            } else {
               self.error("Unsupported bc code: " + bc);
            }
        }
        ip:=ip+1;
        if (ip >= bytecode.size()) {
            if (parent != nil) {
                if (self.isEmpty()) {
                    //OS.stdout().nextPutAll("pushing receiver\n");
                    parent.push(receiver);
                } else {
                    //OS.stdout().nextPutAll("pushing top of stack\n");
                    parent.push(self.pop());
                }
                return parent;
            } else {
                if (self.isEmpty()) {
                    self.push(receiver);
                }
                return -1;
            }
        }
        return nil;
    }
    method processJumpByte(bc) {
        var bytecode := block.getBytecode();
        var jumpMult,jumpBytes;
        ip:=ip+1;
        jumpMult:=bytecode.at(ip);
        ip:=ip+1;
        jumpBytes:=bytecode.at(ip);
        jumpBytes := (jumpMult*256)+jumpBytes;
        if (bc=100) {
            ip := ip + jumpBytes;
        } elsif (bc=101) {
            ip := ip -jumpBytes;
        } else {
            var bv := self.pop();
            if (bc=102) && bv {
                ip:=ip+jumpBytes;
            } elsif (bc=103) && bv {
                ip:=ip-jumpBytes;
            } elsif (bc=104) && bv.not() {
                ip:=ip+jumpBytes;
            } elsif (bc=105) && bv.not() {
                ip:=ip-jumpBytes;
            } else {}
        }
    }
    method processSpecialLiteral(bc) {
        if (bc=112) {
            self.push(receiver);
        } elsif (bc=113) {
            self.push(true);
        } elsif (bc=114) {
            self.push(false);
        } elsif (bc=115) {
            self.push(nil);
        } elsif (bc=116) {
            self.push(self);
        } else {}
    }
    method processScopeLevel() {
        var bytecode := block.getBytecode();
        ip:=ip+1;
        var level := bytecode.at(ip);
        scopeLevel := level;
    }
    method processInstVarFetch() {
        var bytecode := block.getBytecode();
        ip:=ip+1;
        var idx := bytecode.at(ip);
        self.push(receiver.basicAt(idx));
    }
    method processInstVarStore() {
        var bytecode := block.getBytecode();
        ip:=ip+1;
        var idx := bytecode.at(ip);
        receiver.basicPut(idx,self.top());
    }
    method processArrayLiteral() {
        var bytecode := block.getBytecode();
        ip:=ip+1;
        var size := bytecode.at(ip);
        var a := Array.indexedNew(size);
        for (var i:=0;i<size;i:=i+1) {
            a.put(i,self.pop());
        }
        self.push(a);
    }
    method processLiteralFetch() {
        var bytecode := block.getBytecode();
        ip:=ip+1;
        var idx := bytecode.at(ip);
        self.push(block.getLiterals().at(idx));
    }
    method processLocalVarFetch() {
        var bytecode := block.getBytecode();
        ip:=ip+1;
        var scopeIdx := bytecode.at(ip);
        ip:=ip+1;
        var localIdx := bytecode.at(ip);
        self.push(nestedScopes.at(scopeIdx).at(localIdx));
    }
    method processLocalVarStore() {
        var bytecode := block.getBytecode();
        ip:=ip+1;
        var scopeIdx := bytecode.at(ip);
        ip:=ip+1;
        var localIdx := bytecode.at(ip);
        nestedScopes.at(scopeIdx).put(localIdx,self.top());
    }
    method processClassInstVarStore() {
        var bytecode := block.getBytecode();
        ip:=ip+1;
        var idx := bytecode.at(ip);
        receiver.getClass().basicPut(idx,self.top());
    }
    method processClassInstVarFetch() {
        var bytecode := block.getBytecode();
        ip:=ip+1;
        var idx := bytecode.at(ip);
        self.push(receiver.getClass().basicAt(idx));
    }
    method processMsgSend(bc) {
        var bytecode := block.getBytecode();
        var toSuper := false;
        var args;
        var context;
        var newrcvr;

        if (bc=133) {
            toSuper := true;
        }
        ip:=ip+1;
        var numargs := bytecode.at(ip);
        ip:=ip+1;
        var litindex := bytecode.at(ip);
        var selector := block.getLiterals().at(litindex);
        var m;
        args := Array.indexedNew(numargs);
        for (var idx:=numargs-1;idx>=0;idx:=idx-1) {
            args.put(idx,self.pop());
        }
        newrcvr := self.pop();
        
        return self.msgSend(newrcvr,selector,args,toSuper,numargs);
    }
    method msgSend(aReceiver,aSelector,aArgArray,aToSuperFlag,aArgumentCount) {
        var m;
        var untouched := 1000000;
        var primReturn:=[untouched];
        var context;
        
        m := self.lookupMethod(aReceiver,aSelector,aToSuperFlag);
        if (m==nil) {
            var dnaArgs := [aReceiver,aArgArray];
             return self.msgSend(aReceiver,"doesNotUnderstand",dnaArgs,aToSuperFlag,2);
        } else {
            if (aArgumentCount != m.getNumargs()) {
                self.error("Expected " + m.getNumargs() + " arguments for method " +
                    receiver.getClass() + "::" + m.getName() + ",got " +
                    aArgumentCount + " arguments.");
            }
            if (m.getNativeMethodAddress()!=nil) {
                var pr := self.doPrimitive(aReceiver,m,aArgArray,primReturn);
                if (pr==self.primStdReturn()) {
                        if (primReturn.at(0)==untouched) {
                                self.push(aReceiver);
                        } else {
                                self.push(primReturn.at(0));
                        }
                        return nil;
                } elsif (pr==self.primControl()) {
                        return nil;
                } else {}
            }
            
            context := self.getClass().new(); 
            context.setupContext(m.getBlock(),self,aReceiver,aArgArray);
            if (aArgumentCount > 0) {
                var localvars:=context.getNestedScopes().at(0);
                for (var idx:=0;idx < aArgumentCount;idx:=idx+1) {
                    localvars.put(idx,aArgArray.at(idx));
                }
            }
            
            return context;
        }
    }
    primitive method doPrimitive(newrcvr,m,args,returnArray) {
        self.primitiveFailed();
    }
    method processReturn() {
        //OS.stdout().nextPutAll("processing return instruction, parent is " + parent.getPointer()).crlf();
        if (parent != nil) {
            parent.push(self.pop());
        }
    }
    method lookupMethod(aReceiver,aSelector,aToSuperFlag) {
        var startClass,m;
        if (aToSuperFlag) {
            startClass:=aReceiver.getClass().superclass();
        } else {
            startClass:=aReceiver.getClass();
        }
        m:=self.findMethod(startClass,aSelector);
        return m;
    }
    method findMethod(aClass,aSelector) {
        var m;
        if (aClass.isNil()) {
            return nil;
        } else {
            m := aClass.methodDictionary().at(aSelector);
            if (m.isNil()) {
                return self.findMethod(aClass.superclass(),aSelector);
            } else {
                return m;
            }
        }
    }
    method primStdReturn() {
        return 0;
    }
    method primControl() {
        return 2;
    }
    method primFailure() {
        return 3;
    }
class:
    method compileBlockCurrent(aString) {
        return self.compileBlockInContext(thisContext,aString);
    }
    primitive method compileBlockInContext(aContext,aString) {    
        self.primitiveFailed();
    }
}
