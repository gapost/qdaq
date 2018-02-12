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
