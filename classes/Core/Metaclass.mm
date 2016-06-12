namespace Core;

class Metaclass : ClassDescription {
instance:
        attribute thisclass;
        method printString() {
                return thisclass.getName() + " class";
        }
}               
