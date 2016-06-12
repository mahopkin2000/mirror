namespace Collection;

import Core::Character;
import Core::OS;

byte class String : ByteArray {
instance:
        primitive method at(aIndex) {
                self.primitiveFailed();
        }
        primitive method put(aIndex,aCharacter) {
                self.primitiveFailed();
        }
	method size() {
		return super.size() - 1;
	}
//        method + aString {
//                var myNewString,oldsize,size,i,tempstr;
//                tempstr := aString;
//                if (tempstr.getClass() != self.getClass()) {
//                        tempstr := tempstr.printString();
//                }
//                oldsize := self.size();
//                size := self.size() + tempstr.size();
//                myNewString := self.getClass().indexedNew(size);
//                for (i:=0;i<oldsize;i:=i+1) {
//                        myNewString.put(i,self.at(i));
//                }
//                for (i:=self.size();i<size;i:=i+1) {
//                        myNewString.put(i,tempstr.at(i-self.size()));
//                }
//                return myNewString;
//        } 
        method + aString {
                var tempString;
                if (aString.getClass() != self.getClass()) {                        
                        tempString := aString.printString();
                } else {
                        tempString := aString;
                }
                return self.primConcat(tempString);
        }
        primitive method primConcat(aString) {
                self.primitiveFailed();
        }
        primitive method = aString {
                self.primitiveFailed();
        }
        primitive method hash() {
                self.primitiveFailed();
        }
        method isWhiteSpace() {
                var rv := true;
                for (var i:=0;i<self.size();i:=i+1) {
                        var c;
                        c := self.at(i);
                        if (    (c!=Character.tab()) &&
                                (c!=Character.lf()) &&
                                (c!=Character.cr()) &&
                                (c!=Character.space())  ) {
                                rv := false;
                                break;
                        }
                }   
                return rv;
        }
        method printString() {
                return self;
        }
        method indexOf(aCharacter) {
                return self.indexOfStartingAt(aCharacter,0);
        }
        method indexOfStartingAt(aCharacter,aStartIndex) {
                for (var i:=aStartIndex,var s:=self.size();i<s;i:=i+1) {
                        if (self.at(i)==aCharacter) {
                                return i;
                        }        
                }
                return -1;
        }
        method replaceAll(aCharacter,aReplacementCharacter) {
            for (var i:=0,var s:=self.size();i<s;i:=i+1) {
                if (self.at(i)==aCharacter) {
                    self.put(i,aReplacementCharacter);
                }
            }
        }
        method substitute(aTargetString,aReplacementString) {
            var startIndex,idx,scanLength;
            scanLength := aTargetString.size();
            startIndex := self.indexOf(aTargetString.at(0));
            var matchFound := false;
            var newString;
            var i;
            while (matchFound.not()) {
                if (startIndex < 0) {
                    break;
                }
                for (i:=1;i<scanLength;i:=i+1) {
                    if (aTargetString.at(i)!=self.at(startIndex+i)) {
                        break;
                    }
                }
                if (i=scanLength) {
                    matchFound := true;
                } else {
                    startIndex := self.indexOfStartingAt(aTargetString.at(0),startIndex+1);
                }
            }
            if (matchFound) {
                newString := self.copyFrom(0,startIndex-1);
                newString := newString + aReplacementString;
                newString := newString + self.copyFrom(startIndex+scanLength,self.size()-1);
            } else {
					newString := self;
				}
            return newString;                      
        }
        method substring(aStart,aEnd) {
                var len := aEnd - aStart + 1;
                if (len<0) {
                        self.error("end must be greater than or equal to start in substring");
                }
                var rv := self.getClass().indexedNew(len);
                for (var i:=0;aStart<=aEnd;i:=i+1,aStart:=aStart+1) {
                        rv.put(i,self.at(aStart));        
                }
                return rv;
        }
        primitive method asInteger() {
                self.primitiveFailed();
        }
        primitive method asFloat() {
                self.primitiveFailed();
        }
        method asNumber() {
           if (self.indexOf('.') > 0) {
                return self.asFloat();
           } else {
                return self.asInteger();
           }
        }
        method isAtomic() {
            return true;
        }
        method copyFrom(aStart,aEnd) {
            var s:= String.indexedNew(aEnd-aStart+1);
            for (var i:=aStart;i<=aEnd;i:=i+1) {
                s.put(i-aStart,self.at(i));
            }
            return s;
        }
        method tokenize(aToken) {
                var idx:=0;
                var startIdx:=0;
                var tokenVector := Vector.new();
                while ((idx:=self.indexOfStartingAt(aToken,startIdx))>=0) {
                        tokenVector.add(self.copyFrom(startIdx,idx-1));
                        startIdx:=idx+1;
                }
                if (startIdx<self.size()) {
                        tokenVector.add(self.copyFrom(startIdx,self.size()-1));
                }
                return tokenVector;
        }
        method trimWhiteSpace() {
                var startIdx:=0,endIdx:=0;
                for (startIdx:=0;startIdx<self.size();startIdx:=startIdx+1) {
                        if (self.at(startIdx).isWhiteSpace().not()) {
                                break;
                        }
                }
                for (endIdx:=self.size()-1;endIdx>=0;endIdx:=endIdx-1) {
                        if (self.at(endIdx).isWhiteSpace().not()) {
                                break;
                        }
                }
                return self.copyFrom(startIdx,endIdx);
        }
        method tokenizeAsArguments() {
                var vector := self.tokenize(Character.space());
                for (var i:=0;i<vector.size();i:=i+1) {
                        vector.put(i,vector.at(i).trimWhiteSpace());
                }
                return vector;
        }
class:
        method indexedNew(aSize) {
                return super.indexedNew(aSize+1);
        }
        method sprintf(aFormatString,aArgArray) {
            var result;
            result := aFormatString;
            for (var i:=0;i<aArgArray.size();i:=i+1) {
                result := result.substitute("%o",aArgArray.at(i));
            }
            return result;
        }
}
