namespace Collection;

import Core::OS;

class Dictionary : Collection {
instance:
        attribute tally,array;
        
        method initialize() {
                super.initialize();
                tally := 0;
                array := Array.indexedNew(self.getClass().initialSize());
        }
        method capacity(aCapacity) {
                tally := 0;
                array := Array.indexedNew(aCapacity);
        }
        method size() {
                return tally;
        }
		  method isEmpty() {
				return tally=0;
		 }
        method array() {
                return array;
        }
        method at(aKey) {
                var idx,assoc;
                idx := self.indexOfElementOrNil(aKey);
                assoc := array.at(idx);
                if (assoc != nil) {
                        return assoc.getValue();
                } else {
                        return nil;
                }
                
        }
        method put(aKey,aValue) {
                var idx,assoc;
                idx := self.indexOfElementOrNil(aKey);
                if (idx.isNil()) {
                        self.error("dictionary is full");
                }
                assoc := array.at(idx);
                if (assoc != nil) {
                        assoc.setValue(aValue);
                } else {
                        assoc := Association.new();
                        assoc.setKey(aKey);
                        assoc.setValue(aValue);
                        array.put(idx,assoc);
                        tally := tally + 1;
                        if (tally>=(array.size() / self.getClass().ratio())) {
                                var nd,idx,as;
                                as := array.size();
                                nd := self.getClass().capacity(
                                        (as*self.getClass().ratio()).asInteger());
                                for (idx:=0;idx<as;idx:=idx+1) {
                                        assoc := array.at(idx);
                                        if (assoc != nil) {
                                                nd.put(assoc.getKey(),assoc.getValue());
                                        }   
                                }
                                //self.become(nd);
										  array := nd.array();
									     tally := nd.size();
                        }
                }
        }
                
        method indexOfElementOrNil(aKey) {
                var as := array.size();
                var myHash := aKey.hash() % as;
                var idx,elem,assoc;
                for (idx:=myHash;idx<as;idx:=idx+1) {
                        assoc:=array.at(idx);
                        if (assoc==nil) {
                                return idx;
                        }
                        if (assoc != nil) {
                                if (assoc.getKey()=aKey) {
                                        return idx;
                                }
                        }
                }
                for (idx:=0;idx<myHash;idx:=idx+1) {
                        assoc:=array.at(idx);
                        if (assoc==nil) {
                                return idx;
                        }
                        if (assoc != nil) {
                                if (assoc.getKey()=aKey) {
                                        return idx;
                                }
                        }
                }
                return nil;
        }
        method keys() {
                var ka := Array.indexedNew(tally);
                var as := array.size();
                var idx,kidx:=0,assoc;
                for (idx:=0;idx<as;idx:=idx+1) {
                        assoc := array.at(idx);
                        if (assoc != nil) {
                                ka.put(kidx,assoc.getKey());
                                kidx:=kidx+1;
                        }        
                }
                return ka;
        }
        method values() {
                var va := Array.indexedNew(tally);
                var as := array.size();
                var idx,vidx:=0,assoc;
                for (idx:=0;idx<as;idx:=idx+1) {
                        assoc := array.at(idx);
                        if (assoc != nil) {
                                va.put(vidx,assoc.getValue());
                                vidx:=vidx+1;
                        }        
                }
                return va;
        }
        method remove(aKey) {
                var i := self.indexOfElementOrNil(aKey);
                var assoc:=nil;
					 var rv := nil;
                if ((rv:=array.at(i)).isNil().not()) {
						// need to make sure values to the "right"
						// of our hit are properly hashed now that
						// a hole has been opened in the table
						var j := i;
						var k;
               	array.put(i,nil);
						while (true) {
							j := (j+1) % array.size();
							if (array.at(j).isNil()) {
								break;
							}
                		k := array.at(j).getKey().hash() % array.size();
							if ((k<=i) || (k > j)) {
								array.put(i,array.at(j));
								i:=j;
							}
						}
                  rv:=rv.getValue();
						tally := tally - 1;
                }
                return rv;
                
        }
        method contains(aKey) {
                return self.at(aKey).isNil().not();
        }
        method do(aBlock) {
                var idx,as;
                for (idx:=0,as:=array.size();idx<as;idx:=idx+1) {
                        if (array.at(idx) != nil) {
                                aBlock.value([array.at(idx)]);
                        }
                }
        }
        method detect(aBlock) {
                var idx,as,bv:=nil;
                for (idx:=0,as:=array.size();idx<as;idx:=idx+1) {
                        if (array.at(idx) != nil) {
                                bv:=aBlock.value([array.at(idx)]);
                                if (bv==true) {
                                        return array.at(idx);
                                }
                        }
                }
                return nil;
        }
                
class:
        method initialSize() {
                return 20;
        }
        method growSize() {
                return 20;
        }
        method ratio() {
                return 1.5;
        }
        method capacity(aCapacity) {
                var d;
                d := self.new();
                d.capacity(aCapacity);
                return d;
        }  
}
