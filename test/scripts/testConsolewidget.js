
var mainUi = ui.loadTopLevelUi('ui/testconsolewidget.ui','mainUi');

mainUi.findChild("consoleTab").addConsole();

mainUi.show()
