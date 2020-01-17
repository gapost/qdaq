exec("scripts/clearAll.js")


clearAll();

var count = 3;

var gpib = qdaq.appendChild(new QDaqNiGpib("gpib"));

var ret = gpib.open();
if (!ret) throw("GPIB communication error!");
