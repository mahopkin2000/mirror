namespace Database;

import Core::Object;
import Database::OracleDriver;
import Core::Dictionary;

class OracleCursor : Object {
instance:
        attribute connectionHandle,cursorHandle,errorFlag,errorString,bindVariables,colBindings,metadata;
        method initialize() {
                cursorHandle := nil;
                errorFlag := false;
                errorString:="";
                bindVariables:=Dictionary.new();
                colBindings:=nil;
                metadata:=nil;
        }
        method bindVariable(aPosition,aValue) {
                var strvalue := aValue.printString();
                bindVariables.put(aPosition,strvalue);
                OracleDriver.bindVariable(connectionHandle,cursorHandle,aPosition,strvalue);
        }
        method execute() {
                var ra:=[nil,nil];
                OracleDriver.execute(connectionHandle,cursorHandle,ra);
                colBindings:=ra.at(0);
                metadata:=ra.at(1);
        }
        method fetch() {
                return OracleDriver.fetch(connectionHandle,cursorHandle,colBindings);
        }
        method fetch_dict() {
            var arr := self.fetch();
            var dict := Dictionary.capacity(arr.size()*2);
            var cn := self.columnNames();
            for (var i:=0,var as:=arr.size();i<as;i:=i+1) {
                dict.put(cn.at(i),arr.at(i));
            }
            return dict;
        }
        method close() {
                OracleDriver.closeCursor(connectionHandle,cursorHandle);
        }
        method setCursorHandle(aHandle) {
                cursorHandle := aHandle;
        }
        method setConnectionHandle(aHandle) {
                connectionHandle := aHandle;
        }
        method columnNames() {
                if (metadata!=nil) {
                        return metadata.at("colNames");
                } else {
                        return nil;
                }
        }
class:
        method on(aConnectionHandle,aCursorHandle) {
                var nc := self.new();
                nc.setConnectionHandle(aCursorHandle);
                nc.setCursorHandle(aCursorHandle);
                return nc;
        }
}
