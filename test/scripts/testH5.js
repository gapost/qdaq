var dev = qdaq.appendChild(new QDaqDevice('dev'));
var tcp = dev.appendChild(new QDaqTcpip('tcp'));
var loop1 = qdaq.appendChild(new QDaqLoop('loop1'));
var loop2 = qdaq.appendChild(new QDaqLoop('loop1'));

dev.setQDaqProperty('x',true);
tcp.setQDaqProperty('y',1);
tcp.setQDaqProperty('z',[1,2,3]);

dev.interface = tcp;

log("Saving qdaq to h5");
h5write(qdaq,"qdaq.h5");

log("Reading back file");
var t = h5read("qdaq.h5");
qdaq.appendChild(t);

