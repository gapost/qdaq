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

var w = loadUi('ui/testform.ui');
bind(qdaq.loop.t,w.chTime)
bind(qdaq.loop.tst,w.chTest1)
bind(qdaq.loop.tst2,w.chTest2)
w.show()

qdaq.loop.arm()

