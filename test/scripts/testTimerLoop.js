print("Creating Loop");

// create a loop
var loop = new QDaqLoop("loop");
// create a clock channel
var t = new QDaqChannel("t");
t.type = "Clock";
// create a test random channel
var ch1 =  new QDaqChannel("ch1");
ch1.type = "Random";
// create a 2nd channel
var ch2 = new QDaqChannel("ch2");
// create a script job
// that will write to ch2 the square root of ch1
var scr = new QDaqJob("scr");
scr.code = "var v = qdaq.loop.ch1.value(); qdaq.loop.ch2.push(Math.sqrt(v));"
// build the object hierarchy under the root object "qdaq"
loop.appendChild(t);
loop.appendChild(ch1);
loop.appendChild(scr);
loop.appendChild(ch2);
qdaq.appendChild(loop);

loop.createLoopEngine();

print("Tree = \n" + qdaq.objectTree());

var w = loadTopLevelUi('ui/testTimerForm.ui','mainForm');
bind(qdaq.loop.t,w.findChild('t'));
bind(qdaq.loop.ch1,w.findChild('ch1'));
bind(qdaq.loop.ch2,w.findChild('ch2'));

function startPressed(on) {
    if (on) qdaq.loop.arm();
    else qdaq.loop.disarm();
}

startButton = w.findChild("btStart");
startButton.toggled.connect(startPressed);

w.show()

