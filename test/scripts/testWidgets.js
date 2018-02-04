print("Creating Widgets");

var w = loadTopLevelUi('ui/tabwidget.ui','mainForm');

var to = loadUi('ui/testform.ui');
var from = w.findChild('loopCtrl');

w.replaceWidget(from,to);

to = loadUi('ui/deltaControl.ui');
from = w.findChild('deltaCtrl');

w.replaceWidget(from,to);

to = loadUi('ui/cryoTemperatureControl.ui');
from = w.findChild('cryoCtrl');

w.replaceWidget(from,to);

to = loadUi('ui/plotform.ui');
from = w.findChild('plotWdgt');

w.replaceWidget(from,to);

print("Creating Loop");

var loop = new QDaqLoop("loop");

var t = new QDaqTimeChannel("t");
t.format = "Time";
var X1 = QDaqTestChannel("X1");
var X2 = QDaqChannel("X2");
var scr = QDaqJob("scr");
scr.code = "qdaq.loop.X2.push(Math.sqrt(qdaq.loop.X1.value()))";
var Data = QDaqDataBuffer("Data");
Data.type = "Open";
Data.backBufferDepth = 4;
Data.channels = [t, X1, X2];

loop.appendChild(t);
loop.appendChild(X1);
loop.appendChild(scr);
loop.appendChild(X2);
loop.appendChild(Data);


qdaq.appendChild(loop);
loop.createLoopEngine();
loop.period = 200;
loop.limit = 1000;

var loop2 = new QDaqLoop("loop2");
loop2.delay = 5;

t = new QDaqTimeChannel("t");
t.format = "Time";
loop2.appendChild(t);

loop.appendChild(loop2);

print("Tree = \n" + qdaq.objectTree());

bind(qdaq.loop.t,  w.findChild('t'));
bind(qdaq.loop.loop2.t,  w.findChild('t2'));
bind(qdaq.loop.X1, w.findChild('X1'));
bind(qdaq.loop.X2, w.findChild('X2'));

function startPressed(on) {
    if (on) qdaq.loop.arm();
    else qdaq.loop.disarm();
}

startButton = w.findChild("btStart");
startButton.toggled.connect(startPressed);

plot1 = w.findChild("plot1");
Data.updateWidgets.connect(plot1.replot);
plot1.plot(Data.t,Data.X1);

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





