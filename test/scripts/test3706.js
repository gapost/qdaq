
var tcp = qdaq.appendChild(new QDaqTcpip("tcp"));
tcp.host = "100.100.100.20";
tcp.port = 5025;

var k3706 = qdaq.appendChild(new QDaqDevice("k3706"));

var ret = tcp.open() && k3706.on();
if (!ret) throw("Instrumet communication error!");

var prog = "loadscript test\n"+
        "display.clear()\n" +
        "display.settext(\"This is a test\")\n" +
        "print(\"This is a test\")" +
        "endscript\n";

k3706.write(prog);


