import Core::Object;
import Core::OS;
import Collection::Dictionary;
import Core::RegExp;
import IO::File;
import Core::Delay;

class HelloWorld : Object {
instance:
    attribute matt;
class:
    attribute cv1,cv2;
    method main(args) {
	var p1 := {
		var delay:=Delay.forSeconds(2);
		for (var i:=0;i<10;i:=i+1) {
			OS.stdout().nextPutAll("In process 1, waiting for 2 seconds\n");
			delay.wait();
		}
	}.fork([]);
	{
		var delay:=Delay.forSeconds(1);
		for (var i:=0;i<10;i:=i+1) {
			OS.stdout().nextPutAll("In process 2, waiting for 1 seconds\n");
			delay.wait();
		}
	}.fork([]);
	var delay:=Delay.forSeconds(5);
	OS.stdout().nextPutAll("In main process, gonna let my bebe run for a bit\n");
	delay.wait();
	OS.stdout().nextPutAll("Killing bebe 1\n");
	p1.terminate();
	OS.stdout().nextPutAll("Waiting 5 more seconds\n");	
	delay.wait();
	OS.stdout().nextPutAll("All done, I'm outta here!");
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
