//load ui
var w = loadTopLevelUi('ui/burstform.ui','burstForm');

//create a loop
var loop = new QDaqLoop("loop");

//create time channel, append to loop
var t = loop.appendChild(new QDaqChannel("t"));
t.signalName = 'meas. time';
t.type = "Clock";
t.format = "Time";
t.unit = 's';

//create voltage channel
var Vdc =  new QDaqChannel("Vdc");
Vdc.signalName = "Cryostat diode voltage";
Vdc.format = 'FixedPoint';
Vdc.type = 'Random';
Vdc.unit = 'V';
Vdc.digits = 4;
//Vdc.depth = 167;
Vdc.depth = 2*Math.round(1000 / 167); // 2 sec filter
Vdc.averaging = "Running"

//create data buffer
var Data = new QDaqDataBuffer("Data");
Data.type = "Open";
Data.backBufferDepth = 4;
Data.channels = [t, Vdc];
loop.appendChild(Data);


//create gpib interface
//qdaq.appendChild(new QDaqLinuxGpib("gpib"));
qdaq.appendChild(new QDaqNiGpib("gpib"));


//create Keithley 2000 DMM
////var k2000 = new QDaqDevice("k2000");
var k2000 = new QDaqJob("k2000");
//k2000.interface = gpib;
k2000.address = 16;

//create hierarchy
k2000.appendChild(Vdc);
loop.appendChild(k2000);
qdaq.appendChild(loop);
loop.createLoopEngine();
loop.period = 167;

//ui functionality
//"clear"button
btClear = w.findChild('btClear');
btClear.clicked.connect(Data.clear);

//plot1 characteristics
plot1 = w.findChild("plot1");
plot1.timeScaleX = 1;
plot1.plot(Data.t,Data.Vdc,":s");


//plot 1 button
btPlotCh1 = w.findChild('btPlotCh1');
btPlotCh1.toggled.connect(onPlotCh1);

function onPlotCh1(on) {
    var plot1 = ui.burstForm.findChild('plot1');
    var Data = qdaq.loop.Data;
    plot1.clear();
    if (on)
    {
        plot1.plot(Data.t,Data.Vdc);
        initk2000();
    }
}

//start button
startButton = w.findChild("btStart");
startButton.toggled.connect(startPressed);

function startPressed(on) {
    if (on) qdaq.loop.arm();
    else qdaq.loop.disarm();
}

function initk2000(){
k2000.write('*rst');
k2000.write('stat:pres;*cls');
k2000.write('stat:meas:enab 512');
k2000.write('*sre 1');

//k2000.write('trig:coun 20');
k2000.write('trig:coun 1024');

//k2000.write('trac:poin 20');
k2000.write('trac:poin 1024');
k2000.write('trac:feed sens1;feed:cont next');
k2000.write('init');


k2000.write('fetc?');
var buff = k2000.readBytes();


    // convert & store measurement
k2000.Vdc.push(buff.readDoubleLE(2));


/*
k2000.write('*RST');               ‘ Clear registers
k2000.write('*CLS');                       ‘ Clear Model 2000
k2000.write(':INIT:CONT OFF;:ABORT');          ‘ Init off
k2000.write(':SENS:FUNC ‘VOLT:DC’');          ‘ DCV
k2000.write(':SYST:AZER:STAT OFF');           ‘ Auto zero off
k2000.write(':SENS:VOLT:DC:AVER:STAT OFF');     ‘ Filter off
k2000.write(':SENS:VOLT:DC:NPLC 0.01');        ‘ NPLC = 0.01
k2000.write(':SENS:VOLT:DC:RANG 10');          ‘ 10V range
k2000.write(':SENS:VOLT:DC:DIG 4');            ‘ 4 digit
k2000.write(':FORM:ELEM READ');               ‘ Reading only
k2000.write(':TRIG:COUN 1');                 ‘ Trig count 1
k2000.write(':SAMP:COUN 100');                ‘ Sample count 100
k2000.write(':TRIG:DEL 0');                  ‘ No trigger delay
k2000.write(':TRIG:SOUR IMM');                ‘ Immediate trigger
k2000.write(':DISP:ENAB OFF');               ‘ No display
SLEEP 1                                 ‘ Wait one secondPRINT #1,
“:READ?”                       ‘ Read query
LINE INPUT #1, RD$                       ‘ Get data
PRINT RD$                               ‘ Display data
PRINT #1, “:DISP:ENAB ON”                  ‘ Turn on display
‘ Clean up and quit.finish:
CLOSE #1                                ‘ Close file
CLEAR                                  ‘ Interface clear
END
*/
}


w.show()


