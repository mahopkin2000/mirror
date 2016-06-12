namespace Core;

class Block : Object {
instance:
    attribute parent,literals,bytecode,nestedScopesTemplate;
    attribute startScope,sourceArray;
	method getParent() {
		return parent;
	}
	method getLiterals() {
		return literals;
	}
	method getBytecode() {
		return bytecode;
	}
    method getSourceArray() {
        return sourceArray;
    }
    method getNestedScopesTemplate() {
        return nestedScopesTemplate;
    }
}
