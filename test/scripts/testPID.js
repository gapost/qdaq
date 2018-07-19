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

    var corr_c = new QDaqFilter("corr_c");
    corr_c.loadPlugin("lincorr-v0.1");
    corr_c.inputChannels = [t,Tc];
    corr_c.outputChannels = [dTc0,dTcdt];
    corr_c.lincorr.size = 50;

    // create sys
    var sys = new QDaqFilter("sys");
    sys.loadPlugin("fopdt-v0.1");
    sys.inputChannels = [uc];
    sys.outputChannels = [Tc];


    // create PID Temperature controllers
    var tcc = new QDaqFilter("tcc");
    tcc.loadPlugin("pid-v0.1");
    tcc.inputChannels = [Tc];
    tcc.outputChannels = [uc];
    tcc.pid.gain=1
    tcc.pid.Ti=0
    tcc.pid.Td=0
    tcc.pid.b=1
    tcc.pid.Tr=0
    tcc.pid.Nd=5
    tcc.pid.samplingPeriod = 1;
    tcc.pid.maxPower = 10;


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
    tcc.pid.setPoint = v;
    var btn = ui.mainUi.findChild("setTc");
    btn.value = v;
}

function createUi()
{
    var tcUi = loadTopLevelUi('ui/testPID.ui','c');

    var loop = qdaq.findChild('tempCtrlLoop');
    var pid = loop.tcc.pid;
    var fopdt = loop.sys.fopdt;

    // 1st tab
    bind(loop.Tc,tcUi.findChild("Tc"));
    bind(loop.dTcdt,tcUi.findChild("dTcdt"));
    var btn = tcUi.findChild("setTc");
    btn['valueChanged(double)'].connect(setTc);
    bind(pid,"autoMode",tcUi.findChild("autoModeTc"));
    bind(loop.uc,tcUi.findChild("Wc"));
    bind(pid,"power",tcUi.findChild("setWc"));


    // 2nd tab
    bind(pid,"gain",tcUi.findChild("gain"));
    bind(pid,"Ti",tcUi.findChild("Ti"));
    bind(pid,"Td",tcUi.findChild("Td"));
    bind(pid,"Tr",tcUi.findChild("Tr"));
    bind(pid,"beta",tcUi.findChild("beta"));
    bind(pid,"maxPower",tcUi.findChild("maxPower"));

    // 3rd tab
    bind(pid,"autoTune",tcUi.findChild("autoTune"));
    bind(pid,"relayStep",tcUi.findChild("relayStep"));
    bind(pid,"relayThreshold",tcUi.findChild("relayThreshold"));
    bind(pid,"Kc",tcUi.findChild("tunerKc"),true);
    bind(pid,"Tc",tcUi.findChild("tunerTc"),true);

    // fopdt
    bind(fopdt,"kp",tcUi.findChild("Kp"));
    bind(fopdt,"tp",tcUi.findChild("Tp"));
    bind(fopdt,"td",tcUi.findChild("foTd"));

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
    qdaq.mainLoop.dataLoop.bfData.clear();
    qdaq.mainLoop.t.push(0);
}

function startPressed(on) {
    if (on) qdaq.loop.arm();
    else qdaq.loop.disarm();
}



createObjects();
createUi();


qdaq.mainLoop.tempCtrlLoop.arm();
qdaq.mainLoop.dataLoop.arm();
qdaq.mainLoop.arm();
