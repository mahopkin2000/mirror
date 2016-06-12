namespace Core;

import IO::File;

class Debugger : Object {
instance:
class:
    attribute currentContext;
    method debug(aContext) {
        OS.stdout().nextPutAll("Welcome to Mirror Debugger Version 1 - Powered by Reflection\n\n");
        currentContext := aContext;
        self.advanceToNext();
        self.interactive();
    }
    method interactive() {
            var line,cmd;
            var args;
            while (true) {
                OS.stdout().nextPutAll(">>");
                line := OS.stdin().readLine();
                line := self.cleanupCommand(line);
                
                // check if dynamic code
                if (line.at(0)='{') {
                        var block := BlockContext.compileBlockInContext(currentContext,line);
                        OS.stdout().nextPutAll(block.value([]).printString()).crlf();
                        continue;
                }        

                args := line.tokenizeAsArguments();
                args.removeAt(0);
                cmd := line.at(0);
                if (cmd='?') {
                    self.help();
                } elsif (cmd='p') {
                    self.printObject(args);
                } elsif (cmd='q') {
                    break;
                } elsif (cmd='s') {
                    if (self.step()==-1) {
                        OS.stdout().nextPutAll("End of execution\n");
                    }
                } elsif (cmd='n') {
                    if (self.next()==-1) {
                        OS.stdout().nextPutAll("End of execution\n");
                    }
                } elsif (cmd='c') {
                    self.cont();
                } elsif (cmd='b') {
                    self.setBreakPoint(args);
                } else {
                    self.help();
                }
            }
    }
    method cleanupCommand(line) {
        return line.trimWhiteSpace();
    }
    method advanceToNext() {
        var rc;
        while (self.checkSource().not()) {
                rc := currentContext.interpretNextByte(); 
                if (rc==-1) {
                        return -1;
                } elsif (rc.isInstanceOf(BlockContext)) {
                        currentContext := rc;
                } else {}
        }
    }   
    method advanceToNextMessage() {
        var rc;
        while (currentContext.getBlock().getBytecode().
                at(currentContext.getIP()).between(132,133).not()) {
                rc := currentContext.interpretNextByte(); 
                if (rc==-1) {
                        return -1;
                } elsif (rc.isInstanceOf(BlockContext)) {
                        currentContext := rc;
                } else {}
        }
        self.checkSource();
    }   
    
    method args(line) {
        if (line.size() > 3) {
                return line.substring(2,line.size()-3);
        } else {
                return "";
        }
    }
    method help() {
        OS.stdout().nextPutAll("Enter one of the following commands:\n");
        OS.stdout().nextPutAll("\tp <expression> - print the result of evaluating <expression>\n");
        OS.stdout().nextPutAll("\tn - step over message send\n");
        OS.stdout().nextPutAll("\ts - step into message send\n");
        OS.stdout().nextPutAll("\tc - continue execution\n");
        OS.stdout().nextPutAll("\tb - set breakpoint\n");
        OS.stdout().nextPutAll("\tq - quit\n");
        OS.stdout().nextPutAll("\t? - print help information\n");
        OS.stdout().crlf();
    }
    method next() {
        var rc;

        rc := currentContext.interpretNextByte();
        if (rc.isInstanceOf(BlockContext)) {
                var ctxt := rc;
                if (ctxt==currentContext.getParent()) {
                        currentContext := ctxt;
                } else {
                        while (ctxt!=currentContext) {
                                //OS.stdout().nextPutAll("sent standard message-in new context\n");
                                rc := ctxt.interpretNextByte();
                                if (rc.isInstanceOf(BlockContext)) {
                                        //OS.stdout().nextPutAll("dove deeper\n");
                                        ctxt := rc;
                                } 
                        }
                }
        }
        return self.advanceToNextMessage();
    }
    method step() {
        var rc;

        rc := currentContext.interpretNextByte();
        if (rc.isInstanceOf(BlockContext)) {
                //OS.stdout().nextPutAll("got new context in step\n");
                currentContext := rc;
        }
        return self.advanceToNext();
    }

    method cont() {
        var rc;
        while (true) {
            rc := currentContext.interpretNextByte();
            if (rc.isInstanceOf(BlockContext)) {
                currentContext := rc;
            } elsif (rc==-1) {
                //OS.stdout().nextPutAll("End of execution\n");
                break;
            } else {}
        }
    }
    method checkSource() {
        var sa := currentContext.getBlock().getSourceArray();
        if (currentContext.getIP() >= sa.size()) {
                return false;
        }
        var su := sa.at(currentContext.getIP());
        if (su != nil) {
                //OS.stdout().nextPutAll("su="+su).crlf();
                self.showSource();
                return true;
        }
        return false;
    }
    method showSource() {
        var su,sp,sl;
        var m,line;
        var file,rs,fn;
        var lineno,ch;
        var sourcePos;
        var sa := currentContext.getBlock().getSourceArray();
        if (sa.size() > 0) {
            su := sa.at(currentContext.getIP());
            if (su!=nil) {
                sp := su.at(2);
                sl := su.at(1);
                sourcePos := su.at(0);
                m := currentContext.getBlock().getParent();
                if (m!=nil) {
                    fn := m.getSourceFile();
                    file := File.named(fn);
                    rs := file.readStream();
                    rs.seek(sp);
                    OS.stdout().nextPutAll(fn + ":" + sl + ">").crlf();
                    var ch;
                    line := rs.readLine();
                    var i;
                    for (i:=0;i<line.size();i:=i+1) {
                        if (line.at(i).isWhiteSpace().not()) {
                                break;
                         }                        
                    }
                    line := line.copyFrom(i,line.size()-1);
                    OS.stdout().nextPutAll(line).crlf();
                    sourcePos := sourcePos - sp - i;
                    for (i:=0;i<sourcePos;i:=i+1) {
                        OS.stdout().nextPut(Character.space());
                    }
                    OS.stdout().nextPutAll("^\n");
                    rs.close();
                }
            }
        }
    }
    method printObject(args) {
        var idx;
        var rcvr := currentContext.getReceiver();
        var identifier := args.at(0);


        if (identifier="rcvr") {
                self.inspectObject(rcvr);
                return self;
        }
        
        // attempt to look up a local variable of that name
        var sl := currentContext.getScopeLevel();
        var nst := currentContext.getBlock().getNestedScopesTemplate();
        var st := nst.at(sl);
        while (st.isNil().not()) {
                for (var i:=1;i<st.size();i:=i+1) {
                        if (st.at(i)=identifier) {
                                var ns := currentContext.getNestedScopes().at(sl);
                                self.inspectObject(ns.at(i-1));
                                return self;
                        }
                }           
                if (st.at(0).isNil()) {
                        st := nil;
                } else {
                        sl := st.at(0);
                        st := nst.at(st.at(0));
                }
        }
        
        idx := rcvr.getInstanceVariableIndex(identifier);
        if (idx >= 0) {
                self.inspectObject(rcvr.at(idx));
        } else {
                OS.stdout().nextPutAll("Unknown identifier: " + identifier).crlf();
        }
    }
    method inspectObject(aObject) {
                var idx;
                OS.stdout().nextPutAll("a "+aObject.getClass().printString()).crlf();
                var iv := aObject.allInstanceVariables();
                for (var i:=0;i<iv.size();i:=i+1) {
                        idx := aObject.getInstanceVariableIndex(iv.at(i));
                        OS.stdout().nextPutAll("\t"+iv.at(i)+"="+aObject.at(idx)).crlf();
                }
    }
    method setBreakPoint(args) {
        OS.stdout().nextPutAll("args is: " + args).crlf(); 
        
    }
}
