print('qdaq object dynamic props:')
qdaq.A=1;
qdaq.B=2.3;
print(qdaq.listProperties());
print('')

print('new object dynamic props:')
var x = new QDaqChannel("x");
x.A = 1;
print(x.listProperties());
print('')

print('append object and list dynamic props:')
qdaq.appendChild(x);
qdaq.x.B=2;
print(qdaq.x.listProperties());

var t = qdaq.appendChild(new QDaqJob('test'));

t.a = true;
t.b = 1;
t.c = 3.14;
t.d = [1, 2, 3];
t.e = 'george';
t.f = ['george', 'andreas', 'mandy'];

h5write(t,'test.h5');

var t2 = h5read('test.h5');
t2.objectName = 't2';
qdaq.appendChild(t2);
