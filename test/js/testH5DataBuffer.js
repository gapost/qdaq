// create data buffer with 3 columns and fill with data
var bfData = qdaq.appendChild(new QDaqDataBuffer("bfData"));
bfData.type = "Open";

bfData.columnNames = ['A','B','C'];

for(var i=0; i<20; i++) bfData.push([i,i*i,i*i*i]);

// create data buffer with 2 empty columns
var bfData1 = qdaq.appendChild(new QDaqDataBuffer("bfData1"));
bfData1.type = "Open";

bfData1.columnNames = ['A','B'];

log("Saving qdaq to h5");
h5write(qdaq,"qdaq.h5");

log("Reading back file");
var t = h5read("qdaq.h5");
qdaq.appendChild(t);
