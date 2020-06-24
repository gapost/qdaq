log("Creating Widgets");

var w = loadTopLevelUi('ui/resmeas.ui','resmeasCtrl');

function ChanAdd() {
    // These are the two widget names that the user should know (or set) from the ui to
    // be modified:
    // -"Box" tries to find the groupBox where the channels and labels are held
    var Box = w.findChild("groupBox");
    // -User has to know the name of the tabWidget they want to modify
    var tabWidg = w.findChild("tabWidget1");


    var session = qdaq.session0;

    // Find out number of samples already on ui
    var nSamples = 0;
    while (Boolean(w.findChild('R'+(nSamples+1)))) {
        nSamples++;
    }
        //construct object names and text
        var R = loadUi('ui/lonechannel.ui');
        var labelWi = R.findChild("label1")
        var chanWi = R.findChild("R1");


        var chname = "R" + (nSamples+1);
        var lblname = "label" + (nSamples+1);
    // The user also sets the widget name for the inserted widget
    //(so that it can be deleted later)
        var extrname = "whatever" + (nSamples+1);

        var lbltext = "Ch" + (nSamples+1);

        session.rename(chanWi,chname);

        session.rename(R,extrname);

        session.rename(labelWi,lblname);
        labelWi.setText(lbltext);

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

        //put things in place
        session.insertWidget(Box,R);
        session.insertTab(nSamples,newtab,tabtext,tabWidg);
}

function ChanRem() {
    // Find out number of samples already on ui
    var nSamples = 0;
    while (Boolean(w.findChild('R'+(nSamples+1)))) {
        nSamples++;
    }

    //construct names of objects to be deleted (last in - first out)
    var Rname = "R"+nSamples;
    var extraname = "whatever"+nSamples;
    var lblname = "label"+nSamples;

    var Rrem = w.findChild(Rname);
    var lblrem = w.findChild(lblname);

    var extrrem = w.findChild(extraname);
    var tabWidg = w.findChild("tabWidget1");

    var session = qdaq.session0;
    session.deleteWidget(w,Rrem);
    session.deleteWidget(w,lblrem);
    session.deleteWidget(w,extrrem);
    session.deleteTab(nSamples-1,tabWidg);

}


btTabAdd = w.findChild('btChanAdd');
btTabAdd.clicked.connect(ChanAdd);
btTabRem = w.findChild('btChanRem');
btTabRem.clicked.connect(ChanRem);

w.show()





