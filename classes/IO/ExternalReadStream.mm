namespace IO;

import Core::Character;
import Collection::String;

class ExternalReadStream : ExternalStream {
instance:
        method next() {
                return ioConnection.readChar();
        }
        method readLine() {
                var s := String.indexedNew(100);
                var rs := "";
                var c;
                var delim := Character.lf();
                var idx := 0;
                while (c != delim) {
                   c := self.next();
                   if (c.isNil()) {
                      break;
                   }
                   s.put(idx,c);
                   idx:=idx+1;
                   if (idx>99) {
                        idx := 0;
                        rs:=rs+s;
                   }
                }
                rs := rs + s.copyFrom(0,idx-1);
                return rs;
        }
        method nextInto(aByteArray,aLength,aStartIndex) {
                var c;
                var i;
                for (i:=0;i<aLength;i:=i+1) {
                        c:=self.next();
                        if (c!=nil) {
                                aByteArray.put(i+aStartIndex,c);
                        } else {
                                break;
                        }
                } 
                //OS.stdout().nextPutAll("i="+i).crlf();
                return i;
        }
        method nextPut(aCharacter) {
                self.shouldNotImplement();
        } 
        method collectionSpecies() {
                return String;
        }     
}
