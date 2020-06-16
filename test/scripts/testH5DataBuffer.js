// var dev = qdaq.appendChild(new QDaqDevice('dev'));
// var tcp = qdaq.appendChild(new QDaqTcpip('tcp'));

var bfData = qdaq.appendChild(new QDaqDataBuffer("bfData"));
bfData.type = "Open";

bfData.columnNames = ['A','B','C'];

for(var i=0; i<20; i++) bfData.push([i,i*i,i*i*i]);

log("Saving qdaq to h5");
h5write(qdaq,"qdaq.h5");

//log("Reading back file");
//var t = h5read("qdaq.h5");
//qdaq.appendChild(t);
