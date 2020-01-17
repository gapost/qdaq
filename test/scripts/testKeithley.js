exec("scripts/clearAll.js")

function init2182(k2182,rangeIdx,nplc,deltapts) {
    var range = ['0', '1e-2', '1e-1',
            '1', '10', '100'];
    k2182.write("*rst");
    k2182.write("*cls");
    if (rangeIdx) k2182.write(":sens:volt:rang " + range[rangeIdx]);
    else k2182.write(":sens:volt:rang:auto on");
    k2182.write("sens:volt:nplc " + nplc);
    k2182.write("sens:volt:chan1:dfil:stat 0");
    k2182.write("sens:volt:chan1:lpas 0");
    k2182.write("syst:faz 0");
    k2182.write("syst:lsyn 1");
    k2182.write("form sre"); // TODO sre

    k2182.write("trig:del 0");
    k2182.write("trig:sour ext");
    k2182.write("trig:coun " + deltapts);
    k2182.write("samp:coun 1");
    k2182.write("trac:poin " + deltapts);
    k2182.write("trac:feed sens1");
    k2182.write("trac:feed:cont next");
}

function init6220(k6220,Is,deltapts) {
    k6220.write("*rst");
    k6220.write("*cls");
    k6220.write("sour:curr:rang " + Is);
    k6220.write("sour:curr " + Is);
    k6220.write("arm:sour imm");
    k6220.write("arm:dir acc"); 
    k6220.write("trig:sour tlin");
    k6220.write("trig:dir acc");
    //k6220.write("trig:ilin 1");
    k6220.write("trig:outp del");
    k6220.write("sour:swe:spac list");
    k6220.write("sour:swe:rang fix");


    // make the current sweep list
    var lst = [Is];
    for (var i=1; i<deltapts; i++) lst.push((i % 2) ? -Is : Is);
    var ll = lst.shift();
    lst.push(ll);

    var str = "sour:list:curr " + lst[0];
    for (var i=1; i<deltapts; i++) str += "," + lst[i];
    print(str)
    k6220.write(str);
    str = "sour:list:del 0.001";i
    for (var i=1; i<deltapts; i++) str += ",0.001";
    k6220.write(str);
    str = "sour:list:comp 10";
    for (var i=1; i<deltapts; i++) str += ",10";
    k6220.write(str);

    k6220.write("sour:swe:coun inf");
    k6220.write("sour:swe:cab 0");

}

function showClosed() {
    k3706.write('print(channel.getclose("Slot2"))\n');
    var msg = k3706.read();
    k3706.write('print(channel.getclose("Slot6"))\n');
    msg = msg.trim() + ", " + k3706.read();
    return msg;
}


function beamOn(on) {
    k3706.write('digio.writebit(9, ' + on + ')\n')
}

function run() {

    var channels = [2001,2003,2005,2007];
    var sample = ['Sample 2 - Al poly','Sample 1 - W','Sample 3 - Al SC','None']
    var i = count % 4;
    var j = (count-1) % 4;
    count++;
    var msg = 'channel.open(\"' + channels[j] + '\")\nchannel.close(\"' + channels[i] + '\")\n';
    //print(msg)
    k3706.write(msg);
    k3706.write('print(channel.getclose("Slot2"))\n')
    print(k3706.read())
    k2182.write('*wai;trac:data?')
    var buff = k2182.readBytes();
    print("Rec buffer size " + buff.length)
    deltaCalc(buff);
    print("Channel " + channels[j] + ", " + sample[j] +" = " + k2182.R)
    //k2182.write("trac:cle");
    k2182.write("trac:feed:cont next; :init");
    //k2182.write("init");
}

function deltaCalc(buff)
{
    var offset = 2;
    var sp = 0, sm = 0;
    var n = k2182.deltapts;

    for(var i=0; i<n; i++)
    {
        var x = buff.readFloatLE(offset + i*4);
        if (i % 2) sm += x;
        else sp += x;
    }
    var y;
    if (n % 2) {
        var m = (n-1)/2;
        y = (m*sp-(m+1)*sm)/2/m/(m+1);
    }
    else y = (sp-sm)/n;



    k2182.Vr=y;
    k2182.Vth=((sp+sm)/n);
    k2182.R= y / k2182.Is;


}



clearAll();

var count = 3;

var gpib = qdaq.appendChild(new QDaqNiGpib("gpib"));
//gpib.loadPlugin('libqdaqlinuxgpibplugin.so');

// Keithley 2182 NVM
var k2182 = qdaq.appendChild(new QDaqDevice("k2182"));
k2182.interface = gpib;
k2182.address = 7;
//k2182.disarmCode =

// Keithley 6220 Current Source
var k6220 = qdaq.appendChild(new QDaqDevice("k6220"));
k6220.interface = gpib;
k6220.address = 12;

var tcp = qdaq.appendChild(new QDaqTcpip("tcp"));
tcp.host = "100.100.100.20";
tcp.port = 5025;

var k3706 = qdaq.appendChild(new QDaqDevice("k3706"));
k3706.interface = tcp;

var ret = gpib.open() && k2182.on() && k6220.on();
if (!ret) throw("GPIB communication error!");
k6220.write("*rst");
k6220.write("*cls");
k2182.write("*rst");
k2182.write("*cls");

ret = tcp.open() && k3706.on();
if (!ret) throw("LAN communication error!");

//k3706.write('digio.trigger[1].mode = digio.TRIG_FALLING\n');
//k3706.write('digio.trigger[1].pulsewidth=0.1\n')
k3706.write('channel.open("allslots")');

//print('3s wait...')
//wait(3000);

var channels = [1,3,5,7];

var Is = 10e-3;

var rangeIdx = 0;

var nplc = 1.0;

var deltapts = 3;

var delay = 4;



k2182.channels = channels;
k2182.Is = Is;
k2182.deltapts = deltapts;
k2182.buff = new ByteArray();
k2182.R = 0;
k2182.Vr = 0;
k2182.Vth = 0;

print('Config 2182...')
init2182(k2182,rangeIdx,nplc,deltapts);


print('Config 6220...')
init6220(k6220,Is,deltapts);


// k2182.write("trac:data?")




print('Init 2182...')
// init k2182
// goes to trigger wait
k2182.write("init");
//print('3s wait...')
//wait(3000);

// close 1st channel on the 3706
// ....

print('Init 6220...')
// init 6220
// starts the sweep
k6220.write('sour:swe:arm');
k6220.write('init');

print('3s wait...')
//wait(3000);

k2182.write('trig:sign');

//k6220.write("arm:sign");
//print('SOT Trigger...')
//k3706.write('digio.trigger[1].assert()\n');

