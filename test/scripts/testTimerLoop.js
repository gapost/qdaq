function createLoop() {

    log("Creating Loop");

    // create a loop
    var loop = new QDaqLoop("loop");
    loop.period = 100;
    // create a loop
    var loop2 = new QDaqLoop("loop2");
    loop2.delay = 2;


    // CREATE CHANNELS
    // a clock channel
    var t = new QDaqChannel("t");
    t.type = "Clock";
    t.disarmCode = 'this.value(); wait(1000)'
    // a 2nd clock channel
    var t2 = new QDaqChannel("t2");
    t2.type = "Clock";
    // a test random channel
    var ch1 =  new QDaqChannel("ch1");
    ch1.type = "Random";
    // another channel
    var ch2 = new QDaqChannel("ch2");
    // create a script job
    // that will write to ch2 the square root of ch1
    var scr = new QDaqJob("scr");
    scr.runCode = "var v = qdaq.loop.ch1.value(); sleep(50); this.ch2.push(Math.sqrt(v));"
    // yet another channel
    var ch3 = new QDaqChannel("ch3");
    // an interpolator that
    // from ch2 [0,1] goes to ch3 [10,20]
    var interp = new QDaqInterpolator('interp');
    interp.setTable([0,1],[10,20])
    interp.type = 'Linear'
    interp.inputChannels = ch2;
    interp.outputChannels = ch3;


    // create and
    // build the object hierarchy under the root object "qdaq"
    loop.appendChild(t);
    loop.appendChild(ch1);
    loop2.appendChild(t2);
    scr.appendChild(ch2);
    loop2.appendChild(scr);
    loop2.appendChild(interp);
    loop2.appendChild(ch3);
    loop.appendChild(loop2);
    qdaq.appendChild(loop);

    loop.createLoopEngine();

    log("Tree = \n" + qdaq.objectTree());
}


function createUi(uiname) {

    var w = ui.loadTopLevelUi(uiname,'mainForm');
    ui.bind(qdaq.loop.t,w.findChild('t'));
    ui.bind(qdaq.loop.loop2.t2,w.findChild('t2'));
    ui.bind(qdaq.findChild('ch1'),w.findChild('ch1'));
    ui.bind(qdaq.findChild('ch2'),w.findChild('ch2'));
    ui.bind(qdaq.findChild('ch3'),w.findChild('ch3'));

    function startPressed(on) {
        if (on) qdaq.loop.arm();
        else qdaq.loop.disarm();
    }

    function startPressed2(on) {
        if (on) qdaq.loop.loop2.arm();
        else qdaq.loop.loop2.disarm();
    }

    var startButton;
    startButton = w.findChild("btStart");
    startButton.toggled.connect(startPressed);

    startButton = w.findChild("btStart2");
    startButton.toggled.connect(startPressed2);

    w.show()

}

function doit() {
    var cb = ui.comboForm.findChild('comboBox');
    createUi(cb.currentText);
}

importExtension('qdaq-filters')
createLoop();
var w = ui.loadTopLevelUi('ui/testCombo.ui','comboForm');
var bt = w.findChild("pushButton");
bt['clicked()'].connect(doit);
w.show();


/*
log("Saving qdaq to h5");
h5write(qdaq,"qdaq.h5");

log("Reading back file");
var t = h5read("qdaq.h5");
t.objectName = 'cloned_qdaq'
qdaq.appendChild(t);
*/
