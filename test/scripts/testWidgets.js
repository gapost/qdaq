print("Creating Widgets");

var w = loadTopLevelUi('ui/tabwidget.ui','form1');

var to = loadUi('ui/testform.ui');
var from = w.findChild('loopCtrl');

w.replaceWidget(from,to);

to = loadUi('ui/deltaControl.ui');
from = w.findChild('deltaCtrl');

w.replaceWidget(from,to);

to = loadUi('ui/cryoTemperatureControl.ui');
from = w.findChild('cryoCtrl');

w.replaceWidget(from,to);

print("Creating Loop");

var loop = new QDaqLoop("loop");

var t = new QDaqTimeChannel("t");
t.format = "Time";
var X1 = QDaqTestChannel("X1");
var X2 = QDaqChannel("X2");
var scr = QDaqJob("scr");
scr.code = "qdaq.loop.X2.push(qdaq.loop.X1.value())";
var Data = QDaqDataBuffer("Data");
Data.channels = [X1, X2];

loop.appendChild(t);
loop.appendChild(X1);
loop.appendChild(scr);
loop.appendChild(X2);
loop.appendChild(Data);
qdaq.appendChild(loop);
loop.createLoopEngine();

print("Tree = \n" + qdaq.objectTree());

bind(qdaq.loop.t,  w.findChild('t'));
bind(qdaq.loop.X1, w.findChild('X1'));
bind(qdaq.loop.X2, w.findChild('X2'));

function startPressed(on) {
    if (on) qdaq.loop.arm();
    else qdaq.loop.disarm();
}

startButton = w.findChild("btStart");
startButton.toggled.connect(startPressed);

w.show()

