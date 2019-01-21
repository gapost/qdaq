// Testing the Vector JS class

// create some vectors

// an empty vector
var x = new Vector;

// a vector with 10 positions all 0
var y = new Vector(10);

// a Vector initialized by an array
var z = new Vector([1,2,3]);

// print their contents by calling toArray()
// which then calls toString()
print('x = ' + x.toArray())
print('y = ' + y.toArray())
print('z = ' + z.toArray())

// making x circular and pushing data
x.circular = true;
x.capacity = 5;
x.push(y)
x.push(0.1)
x.push(z)
print('x = ' + x.toArray())

// this throws an error in JS
x.capacity = 0;
x.push(1)
