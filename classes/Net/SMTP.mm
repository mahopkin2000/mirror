namespace Net;

import Core::Object;
import Core::OS;
import IO::ExternalReadStream;
import IO::ExternalWriteStream;
import Core::Exception;

class SMTP : Object {
instance:
class:
    method sendMail(aHost,aFromAddress,aToAddress,aSubject,aMessage) {
        var sa := SocketAddress.from(aHost,25);
        var s := Socket.open(Socket.sockStream(),Socket.protoTCP());
        s.connect(sa);
        var rs := s.readStream();
        var ws := s.writeStream();
        var resp;
        // read opening text from server
        resp := rs.readLine();
        OS.stdout().nextPutAll("SERVER: "+resp);
        // send helo string
        OS.stdout().nextPutAll("CLIENT: HELO bubba.com\n");
        ws.nextPutAll("HELO bubba.com\n");
        // read welcome string
        resp := rs.readLine();
        OS.stdout().nextPutAll("SERVER: "+resp);
        // send from string
        OS.stdout().nextPutAll("CLIENT: MAIL FROM: " + aFromAddress + "\n");
        ws.nextPutAll("MAIL FROM: " + aFromAddress + "\n");
        // receive response
        resp := rs.readLine();
        OS.stdout().nextPutAll("SERVER: "+resp);
        // send to string
        OS.stdout().nextPutAll("CLIENT: RCPT TO: " + aToAddress + "\n");
        ws.nextPutAll("RCPT TO: " + aToAddress + "\n");
        // receive response
        resp := rs.readLine();
        OS.stdout().nextPutAll("SERVER: "+resp);
        // send data begin marker
        OS.stdout().nextPutAll("CLIENT: DATA\n");
        ws.nextPutAll("DATA\n");
        // receive response
        resp := rs.readLine();
        OS.stdout().nextPutAll("SERVER: "+resp);
        // send to
        OS.stdout().nextPutAll("CLIENT: To: " + aToAddress + "\n");
        ws.nextPutAll("To: " + aToAddress + "\n");
        // send from
        OS.stdout().nextPutAll("CLIENT: From: " + aFromAddress + "\n");
        ws.nextPutAll("From: " + aFromAddress + "\n");
        // send subject
        OS.stdout().nextPutAll("CLIENT: Subject: " + aSubject + "\n\n");
        ws.nextPutAll("Subject: " + aSubject + "\n\n");
        // send message body
        OS.stdout().nextPutAll("CLIENT: "+aMessage+"\n.\n");
        ws.nextPutAll(aMessage+"\n.\n");
        // receive response
        resp := rs.readLine();
        OS.stdout().nextPutAll("SERVER: "+resp);
        // send quit
        OS.stdout().nextPutAll("CLIENT: QUIT\n");
        ws.nextPutAll("QUIT\n");
        // receive response
        resp := rs.readLine();
        OS.stdout().nextPutAll("SERVER: "+resp);
        s.close();
    }
    // test harness
    method main(args) {
            self.sendMail("photo.mycingular.com","bubba@bubba.com",
                "mahopkin@cisco.com","test message","test message");
    }
}
