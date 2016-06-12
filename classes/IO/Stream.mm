namespace IO;

import Core::Object;
import Core::Character;
import Collection::Array;

// abstract class for all streams
class Stream : Object {
instance:
        attribute collection,position;
        method on(aCollection) {
                collection := aCollection;
                position := 0;
        }
        method nextPutAll(aCollection) {
                for (var idx:=0,var cs:=aCollection.size();idx<cs;idx:=idx+1) {
                        self.nextPut(aCollection.at(idx));
                }
        }  
        method nextPut(aObject) {
                self.subclassResponsibility();
        }
        method next() {
                self.subclassResponsibility();
        }
        method nextItems(aItemCount) {
                var a := self.collectionSpecies().indexedNew(aItemCount);
                for (var idx:=0;idx<aItemCount;idx:=idx+1) {
                        a.put(idx,self.next());
                }
                return a;
        }
        method contents() {
                self.subclassResponsibility();
        }
        method flush() {
                self.subclassResponsibility();
        }
        method cr() {
                self.nextPut(Character.cr());
        }
        method lf() {
                self.nextPut(Character.lf());
        }
        method crlf() {
                self.cr().lf();
        }
        method tab() {
                self.nextPut(Character.tab());
        }
        method collectionSpecies() {
                return Array;
        }
class:
        method on(aCollection) {
                var str;
                str := self.new();
                return str.on(aCollection);
        }
}
