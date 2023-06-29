// Test QDaqObject creation
log("Appending a child test1");
var x = new QDaqJob("test1");
qdaq.appendChild(x);
log("Tree = \n" + qdaq.objectTree());

var splitStr = "\n\n**************************************************\n\n";
log(splitStr)

log("Insert a child test2 in front of test1");
x = new QDaqJob("test2");
qdaq.insertBefore(x,qdaq.test1);
log("Tree = \n" + qdaq.objectTree());

log(splitStr)
log("Insert test3 as child of test2");
x = new QDaqJob("test3");
qdaq.test2.appendChild(x);
log("Tree = \n" + qdaq.objectTree());

log(splitStr)
log("Insert test3 as child of test1");
x = new QDaqJob("test3");
qdaq.test1.appendChild(x);
log("Tree = \n" + qdaq.objectTree());

log(splitStr)
log("Clone test2 and put it as child to test 1");
x = qdaq.test2.clone();
qdaq.test1.appendChild(x);
log("Tree = \n" + qdaq.objectTree());

//log(splitStr)
//log("Remove test 1");
//x = qdaq.test1;
//x = qdaq.removeChild(x);
//log("Tree = \n" + qdaq.objectTree());

//log(splitStr)
//log("Replace test2 with test1");
//x = qdaq.replaceChild(x,qdaq.test2);
//log("Tree = \n" + qdaq.objectTree());

//log(splitStr)
//log("Remove test1")
//qdaq.removeChild(qdaq.test1);
//log("Tree = \n" + qdaq.objectTree());
//log(splitStr)




