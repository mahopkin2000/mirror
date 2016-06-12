namespace Collection;

indexed class Array : Collection {
instance:
        method add(aObject) {
                self.error("Arrays do not support the add operation.");
        }
        method do(aBlock) {
            var rv;
                for (var i:=0,var s:=self.size();i<s;i:=i+1) {
                        rv:=aBlock.value([self.at(i)]);
                }
        }
}
