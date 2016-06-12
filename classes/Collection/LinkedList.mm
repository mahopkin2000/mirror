namespace Collection;

class LinkedList : Collection {
instance:
        attribute head,tail,size;
        method initialize() {
            head := tail := nil;
            size := 0;
        }
        method push(aObject) {
            self.add(aObject);
        }
        method pop() {
            return self.remove(self.last());
        }
        method top() {
            return self.last();
        }
        method add(aObject) {
            var l := self.newNode(aObject);
            if (head.isNil()) {
                head := tail := l;
            } else {
                l.put(0,tail);
                tail.put(1,l);
                tail := l;
            }
            size := size + 1;
        }
        method contains(aObject) {
            return (self.nodeForObject(aObject)) != nil;
        }
        method remove(aObject) {
            var l := self.nodeForObject(aObject);
            if (l != nil) {
                var p := l.at(0);
                var n := l.at(1);
                p.put(1,n);
                n.put(0,p);
                size := size - 1;
                return l.at(2);
            } else {
                return nil;
            }
        }
        method at(aIndex) {
            if (aIndex < 0) || (aIndex >= size) {
                self.error("Index out of bounds: " + aIndex);
            }
            var l := self.nodeAt(aIndex);
            return l.at(2);
        }
        method put(aIndex,aObject) {
            if (aIndex < 0) || (aIndex >= size) {
                self.error("Index out of bounds: " + aIndex);
            }
            var l := self.nodeAt(aIndex);
            l.put(2,aObject);
        }
        method nodeAt(aIndex) {
            var cnt := 0;
            var l := head;
            while (cnt < aIndex) {
                l := l.at(1);
                cnt := cnt + 1;
            }
            return l;
        }
        method nodeForObject(aObject) {
            var l := head;
            while (l != nil) {
                if ( (l.at(2))=aObject) {
                    return l;
                }
                l := l.at(1);
            }
            return nil;
        }
        method size() {
            return size;
        }
        method newNode(aObject) {
            return [nil,nil,aObject];
        }
        method last() {
            return tail.at(2);
        }
        method do(aBlock) {
            var e := head;
            while (e != nil) {
                aBlock.value([e.at(2)]);
                e:=e.at(1);
            }
        }
}
