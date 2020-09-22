// it seems that javascript strings end with the \n character
// but I add it for safe mesure

let el_msg = document.getElementById('msg');
let el_chat = document.getElementById('chat');
let el_addroom = document.getElementById('addroom');
let el_addfriend = document.getElementById('addfriend');
let el_friends = document.getElementById('friends');

let Chatrooms = new Map();
let currentChatroomid = null;
let currentChatroomname = null;
let chatroomlength = 0;
let xmlDoc = createXML("root");
let chatroom = null;
let wsid = Math.floor(Math.random() * 100);
var elUser = xmlDoc.addChild("user", null, null);

elUser.setAttribute("closing", "false");
elUser.setAttribute("id", wsid);

el_msg.focus();

let ws = new WebSocket('ws://'+window.location.host+window.location.pathname);

function validString(str){
	return !/[~`!#$%\^&*+=\-\[\]\\';,/{}|\\":<>\?]/g.test(str);
	//`
}

console.log(getxmlDocStr(xmlDoc));

let userdata = null;

ws.onopen = function(){
	ws.send(getxmlDocStr(xmlDoc));
}

ws.onmessage = function(ev){ 

	data = ev.data.substring(0, ev.data.length-1)+"}";

	userdata = JSON.parse(data);

	if(userdata && userdata.friends){
		el_friends.innerHTML = "";
		userdata.friends.forEach((name, idx) =>{
			console.log(name);

			el_friends.innerHTML += 
				"<p><span class='icon-info'>"+name+"</span></p><br>";
		});
	}

	if(userdata && userdata.error){
		alert(userdata.error);
	}

	if(userdata && userdata.reload){
		console.log("aye");
		window.location.replace(userdata.reload);
	}

	if(userdata.initchatrooms){

		userdata.initchatrooms.forEach((room, idx) => {
			if(idx%2 == 0){
				chatroomlength++;
				Chatrooms.set(room, userdata.initchatrooms[idx+1]);

				let rooms = document.getElementsByClassName('rooms');
				for(let i = 0; i < rooms.length; i++){
					rooms[i].innerHTML += 
						"<p><span class='roomicon'>"+room+"</span></p>"
				}
			}
		});

		let rooms = document.getElementsByClassName('rooms');

		for(let i = 0; i < rooms.length; i++){
			let el_rooms = rooms[i].getElementsByTagName('p');

			for(let i = 0; i < el_rooms.length; i++){
				el_rooms[i].onclick = () => {
					document.getElementById('chat').innerHTML = "";
					currentChatroomname = 
						el_rooms[i].getElementsByTagName('span')[0].innerText;

					chatroom = 
						Chatrooms.get(el_rooms[i]
							.getElementsByTagName('span')[0].innerText);

					currentChatroomid = chatroom;

					el_rooms[i].style.backgroundColor = "#525959";

					for(let j = 0; j < el_rooms.length; j++){
						if(el_rooms[j] != el_rooms[i]){
							el_rooms[j].style.backgroundColor = "#363a3f";
						}
					}

					ws.send("<init_room>"+chatroom+"</init_room>");
				};
			}

		}
	}

	let color = "red";
	if(userdata && userdata.message && userdata.message != ""){
		document.getElementById('chat').innerHTML+=
			'<div class="username" style="color:'+color+'">'+
				userdata.name+
				'<span class="date">'+userdata.date+'</span></div>'+
			'<div class="messageBorder"><p>'+userdata.message+'</p></div>';
	}

	el_chat.scrollTo(0, el_chat.scrollHeight);
};

const xhttp1 = new XMLHttpRequest();

xhttp1.open('GET', '/popups/addroom.html', true);

xhttp1.responseType = 'document';

xhttp1.onload = function(){
	if(xhttp1.readyState = xhttp1.DONE && xhttp1.status == 200){
		let doc = xhttp1.response.getElementById("popupform");

		let buttons = doc.getElementsByTagName("button");

		buttons[0].onclick = function(){
			let addroom_name = doc.getElementsByTagName('input')[0].value;

			let nameExists = false;

			Chatrooms.forEach((id, name) =>{
				if(addroom_name == name){
					nameExists = true;
					return;
				}
			});

			if(nameExists){
				alert('chatroom already exists');
			}

			else if(chatroomlength >= 10){
				alert('maximum amount of chatrooms');
			}

			else if(addroom_name.length > 30){
				alert('maximum number of characters');
			}

			else if(addroom_name.length < 2){
				alert('not enough of characters');
			}

			else if(!validString(addroom_name)){
				alert('special characters not allowed');
			}

			else{
				let elMessage = xmlDoc.getElementsByTagName("message")[0];

				if(elMessage){
					xmlDoc.removeChild(elMessage);
				}
				ws.send("<addroom>"+addroom_name+"</addroom>");
				document.body.removeChild(doc);
			}
		}

		buttons[1].onclick = function(){
			document.body.removeChild(doc);
		}

		el_addroom.onclick = function(){
			document.body.appendChild(doc);
		}

	}
}

xhttp1.send(null);


const xhttp2 = new XMLHttpRequest();

xhttp2.open('GET', '/popups/addfriend.html', true);

xhttp2.responseType = 'document';

xhttp2.onload = function(){
	if(xhttp2.readyState = xhttp2.DONE && xhttp2.status == 200){
		let doc = xhttp2.response.getElementById("popupform");

		let buttons = doc.getElementsByTagName("button");

		buttons[0].onclick = function(){
			
			let name = doc.getElementsByTagName('input')[0].value;

			if(currentChatroomid && name != ""){
				let doc = createXML("addfriend");
				doc.textContent = name;
				doc.setAttribute("id",currentChatroomid);
				doc.setAttribute("name", currentChatroomname);
				ws.send(getxmlDocStr(doc));
			}

			document.body.removeChild(doc);
		}

		buttons[1].onclick = function(){
			document.body.removeChild(doc);
		}

		el_addfriend.onclick = function(){
			document.body.appendChild(doc);
		}

	}
}

xhttp2.send(null);


let shift = false;


const regex_http = RegExp('^http[s]?');
const regex_img = RegExp('.png$|.bmp$|.jpg$');
const regex_yt = RegExp(/www.youtube.com\/watch/);

let isImage = false;

function parseMesage(msg){
	let imagestr = "";

	let imgQuerryIdx = msg.search(".png|.bmp|.jpg");

	if(imgQuerryIdx > 0){
		imagestr = msg.substring(0, imgQuerryIdx+4);
	}else{
		imagestr = msg.substring(0, msg.length-1);
	}

	
	if(regex_yt.test(msg)){
		let strid = msg.substring(msg.indexOf("=")+1, msg.length-1);
		return 
		// "<iframe src='https://www.youtube.com/embed/"+strid+"'> </iframe>"
		"<iframe src='https://www.youtube.com/embed/"+strid+"' /> "
	}

	// works but just remove query string from '?' too the end
	if(regex_http.test(imagestr) && regex_img.test(imagestr)){
	// if(regex_http.test(imagestr) && imgQuerryIdx > 0){
		msg = "<img src='"+imagestr+"'>";
		isImage = true;
		return msg;
	}

	msg = msg.replace(/\n/g, "<br>");
	msg = msg.replace(/\t/g, "&nbsp;&nbsp;&nbsp;&nbsp;");

	let boldmatch = msg.match(/\*\*[^*]*\*\*/g);

	if(boldmatch){
		boldmatch.forEach((str, idx) =>{
			let tmp = str.replace(/\*\*/g, "");
			msg = msg.replace(str, 
				"<span style='font-weight:900;'>"+tmp+"</span>");
		});

	}

	let italicmatch = msg.match(/\*[^*]*\*/g);

	if(italicmatch){
		italicmatch.forEach((str, idx) =>{
			let tmp = str.replace(/\*/g, "");
			msg = msg.replace(str, "<i>"+tmp+"</i>");
		});
	}

	if(msg[0] === ">" && msg[1] === " "){
		msg = "<span style='background-color:#464c53;'>&nbsp;>&nbsp;</span>"+
														msg.substring(1);
	}

	return msg;
}

function sendMessage(element){
	console.log(element.value);
	if(element.value === "" || element.value === "\n" || !chatroom){
		element.value = "";
		return;
	}

	let date = new Date();
	let timestamp = Math.floor(date.getTime()/1000.0);

	let message = parseMesage(element.value);

	var elMessage = xmlDoc.addChild("message", null, null);

	elMessage.setAttribute("chatroom", chatroom);

	elMessage.setAttribute("date", timestamp);

	elMessage.textContent = message;
	console.log(getxmlDocStr(xmlDoc));
	ws.send(getxmlDocStr(xmlDoc));

	xmlDoc.removeChild(elMessage);

	element.value = "";
	element.select();
	element.focus(); 
	element.scrollTo(0, el_chat.scrollHeight);

}

window.addEventListener('focusout', ()=>{
	shift = false;
});


window.addEventListener('beforeunload', (e) =>{
	elUser.setAttribute("closing", "true");

	ws.send(getxmlDocStr(xmlDoc));
	ws.close();
});

function messageKeyup(e){

	switch(e.keyCode){
		case 13: { // enter
			if(!shift){  
				sendMessage(el_msg);
			}
		}
		break;

		case 16: { // shift
			shift = false;
		}
		break;
	}

}
el_msg.addEventListener('keyup', messageKeyup, false);

function messageKeydown(e){
	switch(e.keyCode){
		case 16:{
			shift = true;
		}
		break;

		case 9: { // tab
			e.preventDefault();
			el_msg.value += "\t";
		}
		break;
	}

}

el_msg.addEventListener('keydown', messageKeydown, false);

