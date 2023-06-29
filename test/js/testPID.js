function createObjects() {
    // create main loop
    var mainLoop = new QDaqLoop("mainLoop");

    // add main loop clock
    var t = new QDaqChannel("t");
    t.type = "Inc";
    //t.format = "Time";
    t.signalName = 'time';
    t.unit = 'tick';
    mainLoop.appendChild(t);

    // attach the main loop to qdaq
    mainLoop.createLoopEngine();
    mainLoop.period = 100;
    qdaq.appendChild(mainLoop);

    // create the simulation loop
    var loop = new QDaqLoop("tempCtrlLoop");

    // create the data channels
    var Wc =  new QDaqChannel("Wc");
    Wc.signalName = "Cryostat heater power";
    Wc.format = 'FixedPoint';
    Wc.digits = 3;
    Wc.unit = 'W';

    var uc =  new QDaqChannel("uc");

    var Tc =  new QDaqChannel("Tc");
    Tc.signalName = "Temperature";
    Tc.format = 'FixedPoint';
    Tc.digits = 2;
    Tc.unit = 'K';

    // dT/dt
    var dTcdt =  new QDaqChannel("dTcdt");
    dTcdt.signalName = "Cryostat temperature rate";
    dTcdt.format = 'FixedPoint';
    dTcdt.digits = 2;
    dTcdt.unit = 'K/m';
    dTcdt.multiplier = 60;

    var dTc0 =  new QDaqChannel("dTc0");

    var corr_c = new QDaqLinearCorrelator("corr_c");
    corr_c.inputChannels = [t,Tc];
    corr_c.outputChannels = [dTc0,dTcdt];
    corr_c.size = 50;

    // create sys
    var sys = new QDaqFOPDT("sys");
    sys.inputChannels = [uc];
    sys.outputChannels = [Tc];


    // create PID Temperature controllers
    var tcc = new QDaqPid("tcc");
    tcc.inputChannels = [Tc];
    tcc.outputChannels = [uc];
    tcc.gain=1
    tcc.Ti=20
    tcc.Td=0
    tcc.b=1
    tcc.Tr=0
    tcc.Nd=5
    tcc.samplingPeriod = 1;
    tcc.maxPower = 10;


    // build the object hierarchy under the root object "qdaq"
    loop.appendChild(uc);
    loop.appendChild(sys);
    loop.appendChild(Tc);
    loop.appendChild(corr_c);
    loop.appendChild(dTc0);
    loop.appendChild(dTcdt);
    loop.appendChild(tcc);


    qdaq.mainLoop.appendChild(loop);

    // data loop (period = 1 sec)
    var dataLoop = new QDaqLoop("dataLoop");
    dataLoop.delay = 2; // 1 cycle per sec

    var bfData = new QDaqDataBuffer("bfData");
    bfData.type = "Open";
    bfData.backBufferDepth = 10;

    dataLoop.appendChild(bfData);
    mainLoop.appendChild(dataLoop);

    var chList = [t,Tc,uc];
    bfData.channels = chList;
}

function setTc(v) {
    var tcc = qdaq.findChild('tcc');
    tcc.setPoint = v;
    var btn = ui.mainUi.findChild("setTc");
    btn.value = v;
}

function createUi()
{
    var tcUi = ui.loadTopLevelUi('ui/testPID.ui','mainUi');
    tcUi.setWindowTitle('Testing PID control');

    var loop = qdaq.findChild('tempCtrlLoop');
    var pid = loop.tcc;
    var fopdt = loop.sys;

    // 1st tab
    ui.bind(loop.Tc,tcUi.findChild("Tc"));
    ui.bind(loop.dTcdt,tcUi.findChild("dTcdt"));
    var btn = tcUi.findChild("setTc");
    btn['valueChanged(double)'].connect(setTc);
    ui.bind(pid,"autoMode",tcUi.findChild("autoModeTc"));
    ui.bind(loop.uc,tcUi.findChild("Wc"));
    ui.bind(pid,"power",tcUi.findChild("setWc"));


    // 2nd tab
    ui.bind(pid,"gain",tcUi.findChild("gain"));
    ui.bind(pid,"Ti",tcUi.findChild("Ti"));
    ui.bind(pid,"Td",tcUi.findChild("Td"));
    ui.bind(pid,"Tr",tcUi.findChild("Tr"));
    ui.bind(pid,"beta",tcUi.findChild("beta"));
    ui.bind(pid,"maxPower",tcUi.findChild("maxPower"));

    // 3rd tab
    ui.bind(pid,"autoTune",tcUi.findChild("autoTune"));
    ui.bind(pid,"relayStep",tcUi.findChild("relayStep"));
    ui.bind(pid,"relayThreshold",tcUi.findChild("relayThreshold"));
    ui.bind(pid,"Kc",tcUi.findChild("tunerKc"),true);
    ui.bind(pid,"Tc",tcUi.findChild("tunerTc"),true);

    // fopdt
    ui.bind(fopdt,"kp",tcUi.findChild("Kp"));
    ui.bind(fopdt,"tp",tcUi.findChild("Tp"));
    ui.bind(fopdt,"td",tcUi.findChild("foTd"));

    var bt = tcUi.findChild("btClear");
    bt.clicked.connect(clearPressed);

    var dataLoop = qdaq.findChild('dataLoop');
    var plot = tcUi.findChild("plotTvst");
    plot.clear();
    plot.plot(dataLoop.bfData.t,dataLoop.bfData.Tc)
    plot = tcUi.findChild("plotWvst");
    plot.clear();
    plot.plot(dataLoop.bfData.t,dataLoop.bfData.uc)


    dataLoop.updateWidgets.connect(tcUi.findChild("plotTvst").replot);
    dataLoop.updateWidgets.connect(tcUi.findChild("plotWvst").replot);

    tcUi.show();

}

function clearPressed() {
    qdaq.mainLoop.disarm();
    qdaq.mainLoop.dataLoop.bfData.clear();
    qdaq.mainLoop.t.push(0);
    qdaq.mainLoop.tempCtrlLoop.arm();
    qdaq.mainLoop.dataLoop.arm();
    qdaq.mainLoop.arm();
}

function startPressed(on) {
    if (on) qdaq.loop.arm();
    else qdaq.loop.disarm();
}


importExtension("qdaq-filters")
createObjects();
createUi();


qdaq.mainLoop.tempCtrlLoop.arm();
qdaq.mainLoop.dataLoop.arm();
qdaq.mainLoop.arm();
qdaq.mainLoop.tempCtrlLoop.tcc.autoMode = true
setTc(1)
