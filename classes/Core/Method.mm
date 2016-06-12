namespace Core;

class Method : Object {
instance:
        attribute block,nativeMethodAddress,name,numargs;
        attribute sourceFile,sourceCode;
        
        method getName() {
                return name;
        }
        method getBlock() {
                return block;
        }
        method getNativeMethodAddress() {
                return nativeMethodAddress;
        }
        method getNumargs() {
                return numargs;
        }
        method getSourceFile() {
            return sourceFile;
        }
        method invoke(rcvr,args) {
                return rcvr.perform(self.getName(),args);
        }
}        
