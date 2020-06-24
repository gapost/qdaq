// This test script showcases the use of AddTab/RemoveTab methods, their abilities and limitations.
// In general, when creaqting a tab, you are responsible also for:
// -Not creating conflicts (widgets with the same names as existing ones)
// -Making the new tab functional ("bind"ing, "connect"ing, creating channels etc.).
// The opposite has to happen when deleting a tab.

log("Creating Widget");


var w = loadTopLevelUi('ui/testtab.ui','mainForm');
to = loadUi('ui/buttonform.ui');
from = w.findChild('buttCtrl');
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

function connect(type){
    if (type==0){
        startButton = w.findChild("btStart");
        startButton.toggled.connect(startPressed);

        bind(qdaq.loop.t,  w.findChild('t'));
        bind(qdaq.loop.loop2.t,  w.findChild('t2'));
        bind(qdaq.loop.X1, w.findChild('X1'));
        bind(qdaq.loop.X2, w.findChild('X2'));
        bind(qdaq.loop.X3, w.findChild('qLed'));
    }
    else if (type==1){
        btPlotCh1 = w.findChild('btPlotCh1');
        btPlotCh1.toggled.connect(onPlotCh1);
        btClear = w.findChild('btClear');
        btClear.clicked.connect(Data.clear);
        plot1 = w.findChild("plot1");
        plot1.timeScaleX = 1;
        loop2.updateWidgets.connect(plot1.replot);
        plot1.plot(Data.t,Data.X1,":s");
    }
    else {
        log("Nothing to connect here!!!")
    }
}

function disconnect(type){
    if (type==0){
        startButton = 0;
        startButton.toggled.disconnect(startPressed);

        //do we need an 'unbind' method?
/*        bind(qdaq.loop.t,  w.findChild('t'));
        bind(qdaq.loop.loop2.t,  w.findChild('t2'));
        bind(qdaq.loop.X1, w.findChild('X1'));
        bind(qdaq.loop.X2, w.findChild('X2'));
        bind(qdaq.loop.X3, w.findChild('qLed'));
*/    }
    else if (type==1){
        btPlotCh1 = 0;
        btPlotCh1.toggled.disconnect(onPlotCh1);
        btClear = 0;
        btClear.clicked.disconnect(Data.clear);
        plot1 = 0;
        loop2.updateWidgets.disconnect(plot1.replot);
    }
    else {
        log("Nothing to disconnect here!!!")
    }
}

function startPressed(on) {
    if (on) qdaq.loop.arm();
    else qdaq.loop.disarm();
}

function onPlotCh1(on) {
    var plot1 = ui.mainForm.findChild('plot1');
    var Data = qdaq.loop.Data;
    plot1.clear();
    if (on) plot1.plot(Data.t,Data.X1);
    else plot1.plot(Data.t,Data.X2);
}


function TabAdd() {
    var index = w.findChild('sBindexI').value;
    var name = w.findChild('cBnames').currentText;
    var label = w.findChild('texElabel').text;
    log("Here goes something " + index + " -- " + name + " -- " + label);
    var session = qdaq.session0;
    log("loaded: " + name);
    session.insertTab(index,name,label);
}

function TabRem() {
    var index = w.findChild('sBindexR').value;
    log("Here goes nothing "+index);
    var session = qdaq.session0;
    session.deleteTab(index);
}


function ConnectTab() {
    var tabname = w.findChild('cBnames').currentText;
    var tabIndex = w.findChild('cBnames').currentIndex;
    log("In Connect Tab "+tabname);
    connect(tabIndex);
}
function DisconnTab() {
    var tabname = w.findChild('cBnames').currentText;
    var tabIndex = w.findChild('cBnames').currentIndex;
    log("In Connect Tab "+tabname);
    disconnect(tabIndex);
}

btTabAdd = w.findChild('btTabAdd');
btTabAdd.clicked.connect(TabAdd);
btTabRem = w.findChild('btTabRem');
btTabRem.clicked.connect(TabRem);

btTabAdd = w.findChild('btconnectTab');
btTabAdd.clicked.connect(ConnectTab);
btTabRem = w.findChild('btdisconnTab');
btTabRem.clicked.connect(DisconnTab);


loop2.arm()
w.show()





