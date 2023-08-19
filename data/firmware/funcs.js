function dump() {
    var i, t;
    for (i = -1; ; i--) {
        t = Duktape.act(i);
        if (!t) { break; }
        print(i, t.lineNumber, t.function.name, Duktape.enc('jx', t));
    }
}

function dumpHeapUsage() {
    print("\n")
    print("Heap size: "+getHeapSize())
    print("Heap free: "+getFreeHeap())
    print("Heap min free: "+getMinFreeHeap())
    print("Heap max alloc: "+getMaxAllocHeap())
    print("PSRAM size: "+getPsramSize())
    print("PSRAM free: "+getFreePsram())
    print("PSRAM min free: "+getMinFreePsram())
    print("PSRAM max alloc: "+getMaxAllocPsram())
    print("----------------------------------------\n")
}
