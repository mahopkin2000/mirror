namespace Net;

import Core::Object;
import Core::File;
import Core::OS;
import Core::ByteArray;
import Core::Character;
import Core::Processor;
import Core::Exception;

class FTP : Object {
instance:
    attribute controlConn,dataConn,crs,cws;
    attribute mode;
    method initialize() {
        super.initialize();
        mode := 'A';
    }
    method getLine() {
        var line := crs.readLine();
        OS.stdout().nextPutAll(line);
        return line;
    }
    method getResponse() {
        var line;
        var resp;
        var replyCode;
        var end;
        line := self.getLine();
        replyCode := line.substring(0,2);
        // check to see if this is a multiline or single line response
        if (line.at(3)=='-') {
            end := replyCode+" ";
            resp:=line;
            while (true) {
                line:=self.getLine();
                resp:=resp+line;
                if (line.substring(0,3)=end) {
                    break;
                }
            }
        } else {
            resp := line;
        }
        return [replyCode.asInteger(),resp];
    }
    method sendCommand(aCommand,aParmString) {
        if (aParmString!=nil) {
            OS.stdout().nextPutAll(aCommand + " " + aParmString).crlf();
            cws.nextPutAll(aCommand + " " + aParmString).crlf();
        } else {
            OS.stdout().nextPutAll(aCommand).crlf();
            cws.nextPutAll(aCommand).crlf();
        }
        return self.getResponse();
    }
    method setTransferMode() {
        if (mode='A') {
            if (dataConn!=nil) {
                dataConn.close();
            }
            // set up data connection port
            var ipaddress := controlConn.getSocketAddress().getAddress();

            // bind and listen on the data connection port
            var sa := SocketAddress.from(
                controlConn.getSocketAddress().getAddress(),0);
            dataConn := Socket.open(Socket.sockStream(),Socket.protoTCP());
            dataConn.bind(sa);
            var dataConnPort := dataConn.getSocketAddress().getPort();
            OS.stdout().nextPutAll("Assigned data connection port is: " +
                dataConnPort).crlf();
            dataConn.listen(5);

            ipaddress.replaceAll('.',',');
            var dataConnPortHi := dataConnPort / 256;
            var dataConnPortLow := dataConnPort % 256;
            self.sendCommand("PORT",ipaddress+","+dataConnPortHi+","+dataConnPortLow);
            
        } else {
        }
    }
    method connect(aHostName,aUsername,aPassword) {
        var sa := SocketAddress.from(aHostName,21);
        
        // open socket to FTP server port
        controlConn := Socket.open(Socket.sockStream(),Socket.protoTCP());
        
        // connect the socket
        controlConn.connect(sa);

        // open a readstream and writestream on the socket
        cws := controlConn.writeStream();
        crs := controlConn.readStream();

        // receive welcome
        self.getResponse();

        // send username
        self.sendCommand("USER",aUsername);

        // send password
        self.sendCommand("PASS",aPassword);

        // set transfer mode parameters
        //self.setTransferMode();
    }
    method getFile(aRemoteFilename,aLocalFilename) {
        self.setTransferMode();
        var buff := ByteArray.indexedNew(4096);
        self.sendCommand("RETR",aRemoteFilename);
        var ds := dataConn.accept();
        var address := ds.getPeerAddress();
        OS.stdout().nextPutAll("accepted data connection from " +
            address.getAddress()+"["+address.getPort()+"]\n");

        // open file
        var f := File.named(aLocalFilename);
        var fws := f.writeStream();
        var drs := ds.readStream();
        var ch;
        var bytesRead;
        var fc := fws.getConnection();
        while ((bytesRead:=ds.readInto(buff,0,4096))>0) {
            fc.writeFrom(buff,0,bytesRead);
        }
        fws.close();
        ds.close();
        return self.getResponse();
    }
    method sendFile(aLocalFilename,aRemoteFilename) {
        self.setTransferMode();
        var buff := ByteArray.indexedNew(4096);
        self.sendCommand("STOR",aRemoteFilename);
        var ds := dataConn.accept();
        var address := ds.getPeerAddress();
        OS.stdout().nextPutAll("accepted data connection from " +
            address.getAddress()+"["+address.getPort()+"]\n");

        // open file
        var f := File.named(aLocalFilename);
        var frs := f.readStream();
        var dws := ds.writeStream();
        var ch;
        var bytesRead;
        var fc := frs.getConnection();
        while ((bytesRead:=fc.readInto(buff,0,4096))!=nil) {
            ds.writeFrom(buff,0,bytesRead);
        }
        frs.close();
        ds.close();
        return self.getResponse();
    }
    method changeDirectory(aDirectory) {
        return self.sendCommand("CWD",aDirectory);
    }
    method system() {
        return self.sendCommand("SYST",nil);
    }
    method noop() {
        return self.sendCommand("NOOP",nil);
    }
    method disconnect() {
        return self.sendCommand("QUIT",nil);
    }
    method type(aType) {
        return self.sendCommand("TYPE",aType);
    }
    method pwd() {
        return self.sendCommand("PWD",nil);
    }
    method stat() {
        return self.sendCommand("STAT",nil);
    }
    method passive() {
        mode := 'P';
    }
    method active() {
        mode := 'A';
    }
class:
    attribute opRead;
    method opRead() {
        if (opRead==nil) {
            opRead := Socket.htons(1);
        }
        return opRead;
    }
    method open(aHostName,aUsername,aPassword) {
        var ftp := self.new();
        ftp.connect(aHostName,aUsername,aPassword);
        return ftp;
    }
    method main(args) {
            if (args.size()<3) {
                OS.stderr().nextPutAll("Usage: FTP <hostname> <remote filename> <local filename>\n");
                OS.exit(-1);
            }
            var hostname := args.at(0);
            var remoteFilename := args.at(1);
            var localFilename := args.at(2);
            var ftp := self.open(hostname,"mahopkin","1234");
            ftp.type("I");
            ftp.getFile(remoteFilename,localFilename);
            ftp.sendFile(localFilename,"funk.tar");
            ftp.disconnect();
    }
}
