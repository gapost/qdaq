log("Creating Widgets");

var w = loadTopLevelUi('ui/resmeas.ui','resmeasCtrl');

function ChanAdd() {
    var Box = w.findChild("groupBox");
    var Box2 = w.findChild("groupBox_3");
    var tabWidg = Box2.findChild("tabWidget1");
    var R = loadUi('ui/lonechannel.ui');
    var newtab = loadUi('ui/lonetab.ui')

    var number = w.findChild('sCindex').value;
    var session = qdaq.session0;
    session.insertWidget(Box,R);
    session.insertTab(1,newtab,"Sample 2",tabWidg);
    //for (i =0-->number){
    //      R+i = QLineEdit
    //      R+i label  = QLabel
    //      Layout.addWidget(R+i)
    //      Layout.addWidget(R+i label)
    //      addTab
    //          TabLayout.addWidget
    //}
}

function ChanRem() {
    var Box = w.findChild("groupBox");
    var Box2 = w.findChild("groupBox_3");
    var R = w.findChild("R1");
    var tabWidg = Box2.findChild("tabWidget1");
    var index = w.findChild('sCindex').value;
    log("In Channel Rem "+index);

    var session = qdaq.session0;
    session.deleteWidget(Box,R);
    session.deleteTab(1,tabWidg);

}


btTabAdd = w.findChild('btChanAdd');
btTabAdd.clicked.connect(ChanAdd);
btTabRem = w.findChild('btChanRem');
btTabRem.clicked.connect(ChanRem);

w.show()





