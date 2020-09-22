let xmlHead = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";

function createXML(name){
	let doc= 
		document.implementation.createDocument(null, name)
		.getElementsByTagName(name)[0];

	doc.children = {}

	doc.addChild = function( name, text, attrObj){
		let child = document.createElement(name);

		if(text !== null)
			child.innerText = text;


		if(attrObj !== null){
			Object.entries(attrObj).forEach(([key, value]) => {
				child.setAttribute(key, value);
			})
		}

		child.addChild = this.addChild;
		this.children[name] = child
		this.appendChild(child);
		return child;
	}

	return doc;
}

function getxmlDocStr(doc){
	let docstr = "";
	
	docstr = doc.outerHTML.replace(/ xmlns="[^"]*"/g, "");
	
	console.log(docstr);
	return docstr;
}
