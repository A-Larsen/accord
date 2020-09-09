// it seems that javascript strings end with the \n character
// but I add it for safe mesure
let el_msg = document.getElementById('msg');
let el_chat = document.getElementById('chat');
let el_html = document.getElementsByTagName("html")[0];
let el_addroom = document.getElementById('addroom');
let el_addfriend = document.getElementById('addfriend');
let el_friendsNav = document.getElementById('friends');
el_msg.focus();

let el_msg_rect = msg.getBoundingClientRect();
let el_chat_rect = chat.getBoundingClientRect();

let el_msg_height = el_msg_rect.bottom - el_msg_rect.top;
let el_chat_height = el_chat_rect.bottom - el_chat_rect.top;
let ws = new WebSocket('ws://'+window.location.host+window.location.pathname);

function validString(str){
 return !/[~`!#$%\^&*+=\-\[\]\\';,/{}|\\":<>\?]/g.test(str);
 //`
}

let initheader

let chatroom = null;

let wsid;
wsid = Math.floor(Math.random() * 100);

initheader = "id: "+wsid+"\n"+"name: NULL\n"+"closing: false\n";

let userdata = null;

ws.onopen = function(){
	let date = new Date();
	let timestamp = Math.floor(date.getTime()/1000.0);
	let header = initheader;

	if(chatroom){
		header += "chatroom: "+chatroom+"\n";
	}

	header += "date: "+timestamp+"\n\n";

	ws.send(header);
}


let Chatrooms = new Map();
let currentChatroom = null;
let currentChatroomname = null;
let chatroomlength = 0;

ws.onmessage = function(ev){ 

	data = ev.data.substring(0, ev.data.length-1)+"}";

	userdata = JSON.parse(data);

	if(userdata && userdata.friends){
		el_friendsNav.innerHTML = "";
		userdata.friends.forEach((name, idx) =>{
			console.log(name);

			el_friendsNav.innerHTML += "<p><span class='icon-info'>"+name+"</span></p><br>";
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

				document.getElementById('rooms').innerHTML += "<p><span class='roomicon'>"+room+"</span></p>"

			}
		});

		let el_rooms = document.getElementById('rooms').getElementsByTagName('p');

		for(let i = 0; i < el_rooms.length; i++){
			el_rooms[i].onclick = () => {
				document.getElementById('chat').innerHTML = "";
				currentChatroomname = el_rooms[i].getElementsByTagName('span')[0].innerText;

				chatroom = Chatrooms.get(el_rooms[i].getElementsByTagName('span')[0].innerText);

				currentChatroom = chatroom;

				el_rooms[i].style.backgroundColor = "#525959";
				// el_rooms[i].style.color = "#ffffff";

				for(let j = 0; j < el_rooms.length; j++){
					if(el_rooms[j] != el_rooms[i]){
						el_rooms[j].style.backgroundColor = "#363a3f";
						// el_rooms[j].style.color = "#000000";

					}
				}

				ws.send("initchatroom: "+chatroom+"\n");
			};
		}
	}

	let color = "red";
	if(userdata && userdata.message && userdata.message != ""){
		document.getElementById('chat').innerHTML+='<div class="username" style="color:'+color+'">'+userdata.name+'<span class="date">'+userdata.date+'</span></div><div class="messageBorder"><p>'+userdata.message+'</p></div>';
	}

	el_chat.scrollTo(0, el_chat.scrollHeight);
};

let popup_addroom = document.createElement('div');
popup_addroom.setAttribute("id", "addroomform");
popup_addroom.innerHTML = 
"<h1>add a room</h1><br><br>"+
"create a name <br><input name='addroom_name'/><br><span>(cannot excced 9 chatacters)<span><br>requires login<br>"+
"<button>add room</button>&nbsp;<button>cancel</button><br>";
popup_addroom.style.position = "absolute";
popup_addroom.style.border = "solid";
popup_addroom.style.borderRadius = "5px";
popup_addroom.style.padding = "5px";
popup_addroom.style.bottom = "50%"
popup_addroom.style.left = "45%"
popup_addroom.style.boxShadow =  "5px 10px #232629";
popup_addroom.style.zIndex = "5";
popup_addroom.style.backgroundColor = "#474b53";

let popup_addfriend = document.createElement('div');
popup_addfriend.setAttribute("id", "addfriendform");
popup_addfriend.innerHTML = 
"<h1>add a friend</h1><br><br>"+
"name <br><input name='friend_name'/><br><span>(cannot excced 9 chatacters)<span><br>"+
"<button>add friend</button>&nbsp;<button>cancel</button><br>";
popup_addfriend.style.position = "absolute";
popup_addfriend.style.border = "solid";
popup_addfriend.style.borderRadius = "5px";
popup_addfriend.style.padding = "5px";
popup_addfriend.style.bottom = "50%"
popup_addfriend.style.left = "45%"
popup_addfriend.style.boxShadow =  "5px 10px #232629";
popup_addfriend.style.zIndex = "5";
popup_addfriend.style.backgroundColor = "#474b53";

el_addfriend.onclick = function(){
	document.body.appendChild(popup_addfriend);
}

popup_addfriend.getElementsByTagName('button')[0].onclick = function(){
	name = popup_addfriend.getElementsByTagName('input')[0].value;

	if(currentChatroom && name != ""){
		// console.log(currentChatroom);
		ws.send('addfriend: '+name+"\n"+'friendid: '+currentChatroom+'\nroomname: '+currentChatroomname+"\n\n");
	}

	document.body.removeChild(popup_addfriend);
}

popup_addfriend.getElementsByTagName('button')[1].onclick = function(){
	document.body.removeChild(popup_addfriend);
}

el_addroom.onclick = function(){
	document.body.appendChild(popup_addroom);
}

let addroom_name = null;

popup_addroom.getElementsByTagName('button')[0].onclick = function(){
	addroom_name = popup_addroom.getElementsByTagName('input')[0].value;
	// console.log(addroom_name);

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
		ws.send('addroom: '+addroom_name);
		document.body.removeChild(popup_addroom);
	}
}

popup_addroom.getElementsByTagName('button')[1].onclick = function(){
	document.body.removeChild(popup_addroom);
}



let shift = false;

// == M A R K   D O W N ==
// TODO: make regex for 
// x bold 
// x italic
// - underlined
// - table
// x quote
// - replace \t with "<span sty.."
// etc
//
// == K E Y   C O M M A N S ==
// TODO: 
// - tab key makes a tab
// -

const regex_http = RegExp('^http[s]?');
const regex_img = RegExp('.png$|.bmp$|.jpg$');
// const regex_yt = RegExp('https://www.youtube.com/watch?v=');
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
		return "<iframe width='420' height='315' src='https://www.youtube.com/embed/"+strid+"'> </iframe>"
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
			msg = msg.replace(str, "<span style='font-weight:900;'>"+tmp+"</span>");
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
		msg = "<span style='background-color:#464c53;'>&nbsp;>&nbsp;</span>"+msg.substring(1);
	}

	return msg;
}

el_msg.addEventListener('keyup', (e) => {

	switch(e.keyCode){
		case 13: { // enter
			if(!shift){  
				let date = new Date();
				let timestamp = Math.floor(date.getTime()/1000.0);
				let header = initheader;

				if(chatroom){
					header += "chatroom: "+chatroom+"\n";
				}

				header += "date: "+timestamp+"\n\n";

				// send message in chuncks if it is too long
				let message = parseMesage(el_msg.value);
				let maxchars = 150;
				let con = Math.floor(message.length/maxchars)+1;	

				if(!isImage){
					while(con--){
						ws.send(header+message.substring(0, maxchars));
						message = message.substring(maxchars);
					}
					isImage = false;

				}else{
					ws.send(header+message);
				}

				el_msg.value = "";
				el_msg.select();
				el_msg.focus(); 
				el_chat.scrollTo(0, el_chat.scrollHeight);
			}
		}
		break;

		case 16: { // shift
			shift = false;
		}
		break;
	}
}, false);

el_msg.addEventListener('keydown', (e) => {

	switch(e.keyCode){
		case 16:{
			shift = true;
		}
		break;

		case 9: { // tab
			// el_msg
			e.preventDefault();
			el_msg.value += "\t";
		}
		break;
	}

}, false);

window.addEventListener('focusout', ()=>{
	shift = false;
});

window.addEventListener('beforeunload', (e) =>{
	ws.send('id: '+wsid+'\nclosing: true\n\n');
	ws.close();
});

