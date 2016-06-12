namespace XML;

import Core::Object;
import IO::ExternalReadStream;
import Core::OS;
import Collection::Dictionary;
import Collection::String;
import IO::File;
import Collection::Array;

class ExpatParser : Object {
instance:
        attribute parser,handlers;
        
        method initialize() {
				OS.stderr().nextPutAll("calling create now\n");
                parser := self.create();
                handlers:=Dictionary.new();
                OS.stderr().nextPutAll("finished initialize").crlf();
        }
        method release() {
                self.free();
                parser := nil;
        }
        primitive method create() {
                self.primitiveFailed();
        }
        method free() {
                self.primFree(parser);
        }
        primitive method primFree(aParser) {
                self.primitiveFailed();
        }
        method parse(aReadStream) {
                var ba := String.indexedNew(5000);
                var isFinal := false,numread:=0;
                var ll;
                while (isFinal.not()) {
                        numread:=aReadStream.nextInto(ba,5000,0);
                        //OS.stdout().nextPutAll("after nextInto call").crlf();
                        if (numread < 5000) || (numread==0) {
                                //OS.stdout().nextPutAll("read final block").crlf();
                                isFinal:=true;
                        } 
                        //OS.stdout().nextPutAll("calling primParse with block of size " +
                        //        numread).crlf();
                        ll := Array.indexedNew(2);   
                        self.primParse(parser,ba,numread,isFinal,ll);
                        //OS.stdout().nextPutAll("Finished calling primParse").crlf();
                        var elem:=ll.at(0);
                        while (elem!=nil) {
                            self.perform(elem.at(1),elem.at(2));
                            elem:=elem.at(0);
                        }
                }
                        
        }
        primitive method primParse(aParser,aByteArray,aLength,
            aIsFinalFlag,aLinkedList) {
                self.primitiveFailed();
        }
        method setStartElementHandler(aBlock) {
                handlers.put("startHandler",aBlock);
        }
        method setEndElementHandler(aBlock) {
                handlers.put("endHandler",aBlock);
        }
        method setCharacterDataHandler(aBlock) {
                handlers.put("charHandler",aBlock);
        }
        method handleStartElement(aTagName,aAttributeMap) {
                if (handlers.contains("startHandler")) {
                        handlers.at("startHandler").value([aTagName,aAttributeMap]);
                }
        }
        method handleEndElement(aTagName) {
                if (handlers.contains("endHandler")) {
                        handlers.at("endHandler").value([aTagName]);
                }
        }
        method handleCharacterData(aString) {
                if (handlers.contains("charHandler")) {
                        handlers.at("charHandler").value([aString]);
                }
        }
class:
        method test(args) {
                var filename := args.at(0);
                var f := File.named(filename);
                var s := f.readStream();
                var p := self.new();
				OS.stderr().nextPutAll("created new instance\n");
                p.setStartElementHandler(
                        {tag,attr |
                                OS.stdout().nextPutAll("Start Element: " + tag).crlf();
                                var keys := attr.keys();
                                if (keys.size() > 0) {
                                        OS.stdout().nextPutAll("Attributes are:").crlf();
                                }
                                for (var i:=0;i<keys.size();i:=i+1) {
                                        OS.stdout().tab().nextPutAll(keys.at(i) + "=" + attr.at(keys.at(i))).crlf();
                                }
                                return 0;
                        }
                );
                p.setCharacterDataHandler(
                        {aString |
                                if (aString.isWhiteSpace().not()) {
                                        //OS.stdout().nextPutAll("aString.size() is " + aString.size()).crlf();
                                        OS.stdout().nextPutAll("Content is " + aString).crlf();
                                }
                                return self;
                        }
                );
                p.setEndElementHandler(
                        {tag |
                                OS.stdout().nextPutAll("End Element: " + tag).crlf();
                        }
                );
                                
				OS.stderr().nextPutAll("Calling parse routine\n");
                p.parse(s);
                s.close();
        }
}
