namespace Core;

class ValueModel : Object {
instance:
        attribute value;
        attribute dependents;
        method initialize() {
                dependents := Vector.new();
        }
        method onChangeSend(aSelector,aDependent) {
                dependents.add(Association.keyValue(aSelector,aDependent));
        }
        method on(aValue) {
                value := aValue;
        }
        method getValue() {
                return value;
        }
        method setValue(aValue) {
                dependents.do(
                        {d|d.getValue().perform(d.getKey(),[value]);}
                        );
        }
        
class:
        method on(aValue) {
                var vm := self.new();
                return vm.on(aValue);
        }
        method test(args) {
                var vm;
                var sub;
                sub := Object.new();
                vm := ValueModel.on("fuck off!");
                vm.onChangeSend("error",sub);
                vm.setValue(6);
        }
}
