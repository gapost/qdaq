print("Creating Loop");

var loop = new QDaqLoop("loop");


var t = new QDaqChannel("t");
t.type = "Clock";
var tst =  QDaqChannel("tst");
tst.type = "Random";
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

var w = loadTopLevelUi('ui/testTimerForm.ui','mainForm');
bind(qdaq.loop.t,w.findChild('t'));
bind(qdaq.loop.tst,w.findChild('ch1'));
bind(qdaq.loop.tst2,w.findChild('ch2'));

function startPressed(on) {
    if (on) qdaq.loop.arm();
    else qdaq.loop.disarm();
}

startButton = w.findChild("btStart");
startButton.toggled.connect(startPressed);

w.show()

