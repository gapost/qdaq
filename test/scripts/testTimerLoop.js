print("Creating Loop");
rt.appendChild(new RtTimerLoop("timer"));
with(rt.timer)
{
    appendChild(new RtTimeChannel("t"));
    appendChild(new RtDataBuffer("b"));

    b.channels = [t];
    b.type = "Circular";
    b.capacity = 10;
    b.slave = true;
    t.slave = true;
}

print("Tree = \n" + rt.objectTree());


