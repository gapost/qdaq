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
var tst = QDaqTestChannel("tst");
var tst2 = QDaqChannel("tst2");
var scr = QDaqJob("scr");
scr.code = "qdaq.loop.tst2.push(qdaq.loop.tst.value())"

loop.appendChild(t);
loop.appendChild(tst);
loop.appendChild(scr);
loop.appendChild(tst2);
qdaq.appendChild(loop);
loop.createLoopEngine();

print("Tree = \n" + qdaq.objectTree());

bind(qdaq.loop.t,w.findChild('chTime'));
bind(qdaq.loop.tst,w.findChild('chTest1'));
bind(qdaq.loop.tst2,w.findChild('chTest2'));

function startPressed(on) {
    if (on) qdaq.loop.arm();
    else qdaq.loop.disarm();
}

startButton = w.findChild("btStart");
startButton.toggled.connect(startPressed);

w.show()

