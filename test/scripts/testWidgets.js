log("Creating Widgets");

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

to = loadUi('ui/buttonform.ui');
from = w.findChild('buttCtrl');

w.replaceWidget(from,to);

to = loadUi('ui/resmeas.ui');
from = w.findChild('resmeasCtrl');

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

bind(qdaq.loop.t,  w.findChild('t'));
bind(qdaq.loop.loop2.t,  w.findChild('t2'));
bind(qdaq.loop.X1, w.findChild('X1'));
bind(qdaq.loop.X2, w.findChild('X2'));
bind(qdaq.loop.X3, w.findChild('qLed'));

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


function TabAdd() {
    var index = w.findChild('sBindex').value;
    var name = w.findChild('cBnames').currentText;
    log("Here goes something " + index + " -- " + name );
    var session = qdaq.session0;
    log("loaded: " + name);
//    var mainf = w.findChild("plotForm");
//    session.insertTab(index,mainf,'cpPlot');
    session.insertTab(index,name,'cpPlot');
}

function TabRem() {
    var index = w.findChild('sBindex').value;
    log("Here goes nothing "+index);
    var session = qdaq.session0;
    session.deleteTab(index);
}

function ChanAdd() {
    var Box = w.findChild("groupBox");
    var R = w.findChild("R1");
    var Layout = w.findChild("gridLayout_2");

    var number = w.findChild('sCindex').value;
    var session = qdaq.session0;
    session.insertWidget(Layout,R,"whatever");
    //for (i =0-->number){
    //      R+i = QLineEdit
    //      R+i label  = QLabel
    //      Layout.addWidget(R+i)
    //      Layout.addWidget(R+i label)
    //      addTab
    //          TabLayout.addWidget
    //}
    log("In Channel Add "+number);

}

function ChanRem() {
    var index = w.findChild('sCindex').value;
    log("In Channel Rem "+index);

}


btPlotCh1 = w.findChild('btPlotCh1');
btPlotCh1.toggled.connect(onPlotCh1);

btClear = w.findChild('btClear');
btClear.clicked.connect(Data.clear);

btTabAdd = w.findChild('btTabAdd');
btTabAdd.clicked.connect(TabAdd);
btTabRem = w.findChild('btTabRem');
btTabRem.clicked.connect(TabRem);

btTabAdd = w.findChild('btChanAdd');
btTabAdd.clicked.connect(ChanAdd);
btTabRem = w.findChild('btChanRem');
btTabRem.clicked.connect(ChanRem);

loop2.arm()
w.show()





