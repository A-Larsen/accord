const xhttp = new XMLHttpRequest();

xhttp.open('GET', '/xmlp/test.xml', true);

xhttp.responseType = 'text';

xhttp.onload = function(){
	if(xhttp.readyState = xhttp.DONE && xhttp.status == 200){
		let parser = new DOMParser();
		let xmlDoc = parser.parseFromString(xhttp.response, "text/xml");
		console.log(xmlDoc);
	}
}
xhttp.send(null);
