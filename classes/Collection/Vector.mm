namespace Collection;

class Vector : Collection {
instance:
        attribute lastIndex,array;
        
        method asArray() {
            var arr := Array.indexedNew(self.size());
            var i := 0;
            self.do(
                {e |
                    arr.put(i,e);
                    i:=i+1;
                } );
            return arr;
        }
			method clear() {
				for (var i:=0;i<array.size();i:=i+1) {
					array.put(i,nil);
				}
            lastIndex:=-1;
			}
				
        method at(aIndex) {
                return array.at(aIndex);
        }
        method put(aIndex,aObject) {
                array.put(aIndex,aObject);
        }
		  method first() {
				if (self.size() > 0) {
					return self.at(0);
				} else {
					return nil;
				}
		}
		  method last() {
				if (self.size() > 0) {
					return self.at(lastIndex);
				} else {
					return nil;
				}
			}
		  
        method withCapacity(aCapacity) {
                lastIndex:=-1;
                array:=Array.indexedNew(aCapacity);
        }
        method size() {
                return lastIndex+1;
        }
		  method isEmpty() {
				if (lastIndex < 0) {
					return true;
				} else {
					return false;;
				}
		  }
				
        method add(aObject) {
                if (lastIndex=(array.basicSize()-1)) {
                        self.grow();
                }
                lastIndex:=lastIndex+1;
                array.put(lastIndex,aObject);
        }
        method remove(aObject) {
                var rv:=nil;
                for (var idx:=0,var as:=array.basicSize();idx<as;idx:=idx+1) {
                        if (array.at(idx)=aObject) {
                                rv := self.removeAt(idx);
                                break;
                        }
                }
                return rv;
        }
        method removeAt(aIndex) {
                var rv:=array.at(aIndex);
                array.put(aIndex,nil);
                for (var i:=aIndex+1;i<=lastIndex;i:=i+1) {
                        array.put(i-1,array.basicAt(i));
                }
                lastIndex:=lastIndex-1;
                return rv;
        }
                
        method grow() {
					 var newarray := Array.indexedNew(array.basicSize() +
                        self.getClass().growSize());
                for (var idx:=0;idx<=lastIndex;idx:=idx+1) {
                        newarray.put(idx,self.at(idx));
                }
					array := newarray;
        }
        method do(aBlock) {
                for (var idx:=0;idx<=lastIndex;idx:=idx+1) {
                        aBlock.value([array.at(idx)]);
                }
        }
        method indexOf(aObject) {
                for (var idx:=0;idx<=lastIndex;idx:=idx+1) {
                        if (self.at(idx)=aObject) {
                                return idx;
                        }
                }
                return -1;
        }
class:
        method growSize() {
                return 10;
        }  
        method new() {
                var nc := super.new();
                nc.withCapacity(10);
                return nc;
        }
        method withCapacity(aCapacity) {
                var nc := super.new();
                nc.withCapacity(aCapacity);
                return nc;
        }
}
