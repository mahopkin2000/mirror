namespace Collection;

import Core::Object;

class Collection : Object {
instance:
        method do(aBlock) {
                self.subclassResponsibility();
        }
        method addAll(aCollection) {
                aCollection.do({e|self.add(e);});
        }
        
}
