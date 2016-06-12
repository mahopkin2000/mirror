namespace IO;

class ExternalWriteStream : ExternalStream {
instance:
        method nextPut(aCharacter) {
                ioConnection.writeChar(aCharacter);
        } 
        method nextPutAll(aCollection) {
                ioConnection.writeFrom(aCollection,0,aCollection.size());
        } 
        method next() {
                self.shouldNotImplement();
        }      
}
