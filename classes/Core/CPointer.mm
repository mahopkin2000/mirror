namespace Core;

import Core::Object;

class CPointer : Object {
instance:
	attribute bytes,type;
	method getBytes() {
		return bytes;
	}
	method getType() {
		return type;
	}
}
