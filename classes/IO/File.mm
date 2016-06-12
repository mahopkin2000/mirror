namespace IO;

import Core::Object;

class File : Object {
instance:
        attribute pathname;
        
        method named(aPathname) {
                pathname := aPathname;
        }
        method writeStream() {
                return ExternalWriteStream.on(
                        FileConnection.open(pathname,"w"));
        }
        method delete() {
                self.primDelete(pathname);
        }
        primitive method primDelete(aPathname) {
                self.primitiveFailed();
        }
        method exists() {
            return self.primExists(pathname);
        }
        primitive method primExists(aPathname) {
            self.primitiveFailed();
        } 
        method readStream() {
                return ExternalReadStream.on(
                        FileConnection.open(pathname,"r"));
        }
        method writeAppendStream() {
                return ExternalWriteStream.on(
                        FileConnection.open(pathname,"a"));
        }
class:
        method named(aPathname) {
                var file;
                
                file := self.new();
                file.named(aPathname);
                return file;
        }
                
        method test() {
                var file := File.named("metadata");
                var str := file.writeStream();
                var idx:=0;
                while (idx < 100) {
                        str.nextPutAll("Hello, World!");
                        str.cr().lf();   
                        idx := idx +1;
                }
                str.close();
        }
        method testDelete() {
                var file := File.named("metadata");
                file.delete();
        }
}               
