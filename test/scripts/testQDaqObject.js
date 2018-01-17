// Test QDaqObject creation
print("Appending a child test1");
var x = new QDaqJob("test1");
qdaq.appendChild(x);
print("Tree = \n" + qdaq.objectTree());

var splitStr = "\n\n**************************************************\n\n";
print(splitStr)

print("Insert a child test2 in front of test1");
x = new QDaqJob("test2");
qdaq.insertBefore(x,qdaq.test1);
print("Tree = \n" + qdaq.objectTree());

print(splitStr)
print("Insert test3 as child of test2");
x = new QDaqJob("test3");
qdaq.test2.appendChild(x);
print("Tree = \n" + qdaq.objectTree());

print(splitStr)
print("Insert test3 as child of test1");
x = new QDaqJob("test3");
qdaq.test1.appendChild(x);
print("Tree = \n" + qdaq.objectTree());

print(splitStr)
print("Clone test2 and put it as child to test 1");
x = qdaq.test2.clone();
qdaq.test1.appendChild(x);
print("Tree = \n" + qdaq.objectTree());

print(splitStr)
print("Remove test 1");
x = qdaq.test1;
x = qdaq.removeChild(x);
print("Tree = \n" + qdaq.objectTree());

print(splitStr)
print("Replace test2 with test1");
x = qdaq.replaceChild(x,qdaq.test2);
print("Tree = \n" + qdaq.objectTree());

print(splitStr)
print("Remove test1")
qdaq.removeChild(qdaq.test1);
print("Tree = \n" + qdaq.objectTree());
print(splitStr)




