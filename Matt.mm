import Core::Object;
import Core::OS;
import Collection::Dictionary;
import Core::RegExp;
import Matt:Test;

class HelloWorld : Object {
instance:
    attribute matt;
class:
    attribute cv1,cv2;
    method main(args) {
		for (var i:=0;;i:=i+1) {
			OS.stdout().nextPutAll("Hello, Kylie Hopkins! ");
		}
    }
	method add(args) {
		var l,r;
		
		l := args.at(0).asInteger();
		r := args.at(1).asInteger();
		var s := l + r;
		OS.stdout().nextPutAll("Kylie, the answer is: " + s).crlf();
	}
	method dict(args) {
		var d := Dictionary.new();
		d.put("mattkey",args.at(0));
		var val := d.at("mattkey");
		OS.stdout().nextPutAll("val is: " + val);
	}
	method regexp(args) {
		var re := RegExp.compile("(Kylie)","");
		var matches := re.match("Kylie Hopkins");
		OS.stdout().nextPutAll("matches is: " + matches).crlf();
		for (var i:=0;i<matches.size();i:=i+1) {
			OS.stdout().nextPutAll("matches["+i+"]="+matches.at(i)+"\n");
		}
	}

}
