namespace Core;

class Point : Object {
instance:
        method getX()
        {
                return x;
        }
        method setX(aNumber)
        {
                x := aNumber;                
        }         
        method getY()
        {
                return y;
        }
        method setY(aNumber)
        {
                y := aNumber;
        }   
        method printString() {
                return x.printString() + "@" + y.printString();
        }      

        attribute x,y;
class:
        method fromCoordinates(aXCoord,aYCoord)
        {
                var newPoint;
                var string;
                string := "this is a test string";
                newPoint := self.new().setX(aXCoord);
                newPoint.setX(aXCoord);
                newPoint.setY(aYCoord);
                return newPoint;
        }
        method test(args) {
                OS.stdout().nextPutAll("Creating new point").crlf();
                var p := Point.fromCoordinates(10,10);
                OS.stdout().nextPutAll("Created new point: "+p).crlf();
        }
                
}

