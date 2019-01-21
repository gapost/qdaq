var dev = qdaq.appendChild(new QDaqDevice('dev'));
var tcp = qdaq.appendChild(new QDaqTcpip('tcp'));

dev.interface = tcp;

print("Saving qdaq to h5");
h5write(qdaq,"qdaq.h5");

print("Reading back file");
var t = h5read("qdaq.h5");
qdaq.appendChild(t);

