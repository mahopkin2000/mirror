namespace Core;

import Collection::Vector;

class ClassDescription : Behavior {
instance:
        attribute instvars;
        
        method getInstanceVariables() {
                return instvars;
        }
        method allInstanceVariables() {
                var ivVector := Vector.new();
                self.buildInstVarVector(ivVector);
                return ivVector;
        }
        method getInstanceVariableIndex(aInstVarName) {
                var ivVector := Vector.new();
                self.buildInstVarVector(ivVector);
                return ivVector.indexOf(aInstVarName);
                
        }
        method buildInstVarVector(aVector) {
                if (superclass!=nil) {
                        superclass.buildInstVarVector(aVector);
                        aVector.addAll(self.getInstanceVariables());
                } else {
                        aVector.addAll(self.getInstanceVariables());
                } 
        } 
}               
