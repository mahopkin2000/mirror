namespace Core;

class AspectAdapter : Object {
instance:        
        attribute subject,getter,setter;
        method setValue(aValue) {
                subject.perform(setter,[aValue]);
        }
        method getValue() {
                return subject.perform(getter,[]);
        }
        method on(aSubject,aGetter,aSetter) {
                subject := aSubject;
                getter := aGetter;
                setter := aSetter;
        }        
class:
        method on(aSubject,aGetter,aSetter) {
                var aa := self.new();
                return aa.on(aSubject,aGetter,aSetter);
        } 
}
