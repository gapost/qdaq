log("Creating Widgets");

var w = loadTopLevelUi('ui/resmeas.ui','resmeasCtrl');

function ChanAdd() {

    var Box = w.findChild("groupBox");
    var Box2 = w.findChild("groupBox_3");
    var tabWidg = Box2.findChild("tabWidget1");


    var session = qdaq.session0;

    // Find out number of samples/channels from ui
    var nSamples = 0;
    while (Boolean(w.findChild('R'+(nSamples+1)))) {
        nSamples++;
    }
        var R = loadUi('ui/lonechannel.ui');
        var labelWi = R.findChild("label1")
        var chanWi = R.findChild("R1");
        var chname = "R" + (nSamples+1);
        var lblname = "label" + (nSamples+1);
        var lbltext = "Ch" + (nSamples+1);

        session.rename(chanWi,chname);
        session.rename(labelWi,lblname);
        labelWi.setText(lbltext);

        session.insertWidget(Box,R);

        var newtab = loadUi('ui/lonetab.ui')
        var HwChanWi = newtab.findChild("HwChan1");
        var CurSourWi = newtab.findChild("CurSour1");
        var cbPlotWi = newtab.findChild("cbPlt1");
        var CurSourname = "CurSour" + (nSamples+1);
        var HwChname = "HwChan" + (nSamples+1);
        var cbPlname = "cbPlt" + (nSamples+1);
        var tabtext = "Sample " + (nSamples+1);

        session.rename(HwChanWi,HwChname);
        session.rename(CurSourWi,CurSourname);
        session.rename(cbPlotWi,cbPlname);
        session.insertTab(nSamples,newtab,tabtext,tabWidg);
}

function ChanRem() {
    var Box = w.findChild("groupBox");
    var Box2 = w.findChild("groupBox_3");

    // Find out number of samples/channels from ui
    var nSamples = 0;
    while (Boolean(w.findChild('R'+(nSamples+1)))) {
        nSamples++;
    }
    log("nSamples= "+nSamples);
    var Rname = "R"+nSamples;
    log("Rname= "+Rname);
// TODO : delete tries to delete based on overall GroupBox layout; but this widget has its
// own, in order to appear nice...
    var Rrem = w.findChild(Rname);
    var tabWidg = Box2.findChild("tabWidget1");

    var session = qdaq.session0;
    session.deleteWidget(Box,Rrem);
    session.deleteTab(nSamples-1,tabWidg);

}


btTabAdd = w.findChild('btChanAdd');
btTabAdd.clicked.connect(ChanAdd);
btTabRem = w.findChild('btChanRem');
btTabRem.clicked.connect(ChanRem);

w.show()





