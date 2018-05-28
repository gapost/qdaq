
function init2182(k2182,rangeIdx,nplc,deltapts) {
    var range = ['0', '1e-2', '1e-1',
            '1', '10', '100'];
    k2182.write("*rst");
    if (rangeIdx) k2182.write(":sens:volt:rang" + range[rangeIdx]);
    else k2182.write(":sens:volt:rang:auto on");
    k2182.write("sens:volt:nplc " + nplc);
    k2182.write("sens:volt:chan1:dfil:stat 0");
    k2182.write("sens:volt:chan1:lpas 0");
    k2182.write("syst:faz 0");
    k2182.write("syst:lsyn 1");
    k2182.write("form sre");

    k2182.write("trig:del 0");
    k2182.write("trig:sour ext");
    k2182.write("trig:coun " + deltapts);
    k2182.write("samp:coun 1");
    k2182.write("trac:poin " + deltapts);
    k2182.write("trac:feed sens1");
    k2182.write("trac:feed:cont next");
}

function init6220(k6220,Is,deltapts) {
    k6220.write("");
    k6220.write("*rst");
    k6220.write("sour:curr:rang" + Is);
    k6220.write("sour:curr" + Is);
    k6220.write("arm:sour nst");
    k6220.write("arm:dir sour"); // bypass 1st arm trigger
    k6220.write("trig:sour tlin");
    //k6220.write("trig:ilin 1");
    k6220.write("trig:outp del");
    k6220.write("sour:swe:spac list");
    k6220.write("sour:swe:rang fix");
    var str = "sour:list:curr " + Is;
    for (var i=1; i<deltapts; i++)
    {
        str += ",";
        str += (i % 2) ? (-Is) : Is;
    }
    k6220.write(str);
    str = "sour:list:del 0.001";
    for (var i=1; i<deltapts; i++) str += ",0.001";
    k6220.write(str);
    str = "sour:list:comp 10";
    for (var i=1; i<deltapts; i++) str += ",10";
    k6220.write(str);
    k6220.write("sour:swe:coun inf");
    k6220.write("sour:swe:cab 0");

}

var gpib = qdaq.appendChild(new QDaqGpib("gpib"));

// Keithley 2182 NVM
var k2182 = qdaq.appendChild(new QDaqDevice("k2182"));
k2182.interface = gpib;
k2182.address = 7;

// Keithley 6220 Current Source
var k6220 = qdaq.appendChild(new QDaqDevice("k6220"));
k6220.interface = gpib;
k6220.address = 12;

var ret = gpib.open() && k2182.on() && k6220.on();
if (!ret) throw("Instrumet communication error!");


var channels = [1,3,5,7];

var Is = 1e-9;

var rangeIdx = 1;

var nplc = 1.0;

var deltapts = 3;

var delay = 4;



k2182.channels = channels;
k2182.Is = Is;

init2182(k2182,rangeIdx,nplc,deltapts);

init6220(k6220,Is,deltapts);


// k2182.write("trac:data?")





// init k2182
// goes to trigger wait
k2182.write("init");
k2182.channel = channel;

// close 1st channel on the 3706
// ....

// init 6220
// starts the sweep
k6220.write('init');

