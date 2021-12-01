log("Creating Widgets");

var w = ui.loadTopLevelUi('ui/tabwidget.ui','mainForm');

var to = ui.loadUi('ui/testform.ui');
var from = w.findChild('loopCtrl');

w.replaceWidget(from,to);

to = ui.loadUi('ui/deltaControl.ui');
from = w.findChild('deltaCtrl');

w.replaceWidget(from,to);

to = ui.loadUi('ui/cryoTemperatureControl.ui');
from = w.findChild('cryoCtrl');

w.replaceWidget(from,to);

to = ui.loadUi('ui/plotform.ui');
from = w.findChild('plotWdgt');

w.replaceWidget(from,to);


log("Creating Loop");

var loop = new QDaqLoop("loop");

var t = new QDaqChannel("t");
t.type = "Clock";
var X1 = new QDaqChannel("X1");
X1.type = "Random";
var X2 = new QDaqChannel("X2");
var X3 = new QDaqChannel("X3");
var scr = new QDaqJob("scr");
scr.code = "qdaq.loop.X2.push(Math.sqrt(qdaq.loop.X1.value()));qdaq.loop.X3.push(qdaq.loop.X1.value()>0.5);";
var Data = new QDaqDataBuffer("Data");
Data.type = "Open";
Data.backBufferDepth = 4;
Data.channels = [t, X1, X2, X3];

loop.appendChild(t);
loop.appendChild(X1);
loop.appendChild(scr);
loop.appendChild(X2);
loop.appendChild(X3);
loop.appendChild(Data);


qdaq.appendChild(loop);
loop.createLoopEngine();
loop.period = 20;
loop.limit = 1000;

var loop2 = new QDaqLoop("loop2");
loop2.delay = 50;

t = new QDaqChannel("t");
t.type = "Clock";
loop2.appendChild(t);

loop.appendChild(loop2);

log("Tree = \n" + qdaq.objectTree());

ui.bind(qdaq.loop.t,  w.findChild('t'));
ui.bind(qdaq.loop.loop2.t,  w.findChild('t2'));
ui.bind(qdaq.loop.X1, w.findChild('X1'));
ui.bind(qdaq.loop.X2, w.findChild('X2'));
ui.bind(qdaq.loop.X3, w.findChild('qLed'));

function startPressed(on) {
    if (on) qdaq.loop.arm();
    else qdaq.loop.disarm();
}

startButton = w.findChild("btStart");
startButton.toggled.connect(startPressed);

plot1 = w.findChild("plot1");
plot1.timeScaleX = 1;
loop2.updateWidgets.connect(plot1.replot);
//Data.updateWidgets.connect(plot1.replot);
plot1.plot(Data.t,Data.X1,":s");

function onPlotCh1(on) {
    var plot1 = ui.mainForm.findChild('plot1');
    var Data = qdaq.loop.Data;
    plot1.clear();
    if (on) plot1.plot(Data.t,Data.X1);
    else plot1.plot(Data.t,Data.X2);
}



btPlotCh1 = w.findChild('btPlotCh1');
btPlotCh1.toggled.connect(onPlotCh1);

btClear = w.findChild('btClear');
btClear.clicked.connect(Data.clear);


loop2.arm()
w.show()





