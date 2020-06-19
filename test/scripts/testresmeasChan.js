log("Creating Widgets");

var w = loadTopLevelUi('ui/resmeas.ui','resmeasCtrl');

function ChanAdd() {
    var Box = w.findChild("groupBox");
    var R = w.findChild("R1");
    var Layout = w.findChild("gridLayout_2");

    var number = w.findChild('sCindex').value;
    var session = qdaq.session0;
    session.insertWidget(Box,Layout,R,"whatever");
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


btTabAdd = w.findChild('btChanAdd');
btTabAdd.clicked.connect(ChanAdd);
btTabRem = w.findChild('btChanRem');
btTabRem.clicked.connect(ChanRem);

w.show()





