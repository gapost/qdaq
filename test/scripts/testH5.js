print("Creating child objects");
var x = new QDaqObject("test1");
qdaq.appendChild(x);
x = new QDaqObject("test2");
qdaq.insertBefore(x,qdaq.test1);
x = new QDaqJob("test3");
qdaq.test2.appendChild(x);
x = qdaq.test2.clone();
qdaq.test1.appendChild(x);
print("Tree = \n" + qdaq.objectTree());


print("Saving qdaq to h5");
qdaq.h5write("qdaq.h5");

