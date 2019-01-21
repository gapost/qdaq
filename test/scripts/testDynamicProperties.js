print('qdaq object dynamic props:')
qdaq.A=1;
qdaq.setQDaqProperty('B',2.3);
print(qdaq.listProperties());
print('')

print('new object dynamic props:')
var x = new QDaqChannel("x");
x.setQDaqProperty('A',1);
print(x.listProperties());
print('')

print('append object and list dynamic props:')
qdaq.appendChild(x);
qdaq.x.setQDaqProperty('B',2);
print(qdaq.x.listProperties());

var t = qdaq.appendChild(new QDaqJob('test'));

t.setQDaqProperty('a',true);
t.setQDaqProperty('b',1);
t.setQDaqProperty('c',3.14);
t.setQDaqProperty('d',[1,2,3]);
t.setQDaqProperty('e','george');
t.setQDaqProperty('f',['george','andreas','mandy']);

h5write(t,'test.h5');

var t2 = h5read('test.h5');
t2.objectName = 't2';
qdaq.appendChild(t2);
