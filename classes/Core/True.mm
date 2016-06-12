namespace Core;

class True : Boolean {
instance:
        method || aBoolean {
                return self;
        }
        
        method && aBoolean {
                return aBoolean;
        }

        method and(aBlock) {
            return aBlock.value([]);
        }

        method or(aBlock) {
            return self;
        }
        
        method not() {
                return false;
        } 
        method printString() {
                return "true";
        }             
}
