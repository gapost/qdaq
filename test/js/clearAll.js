function clearAll() {
	var lst = qdaq.children();
	while(lst.length) {
		var d = lst.pop();
		qdaq.removeChild(d);
	}
	gc()
}