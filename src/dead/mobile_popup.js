let el_mobile_navhead = document.getElementById('mobile-navhead');
let el_mobile_navIcon = document.getElementById('mobile-nav-icon');
let el_mobile_nav = document.getElementById('mobile-nav');

let popup_mobile_createMessage = document.createElement('div');
popup_mobile_createMessage.setAttribute("class", "popupform");
popup_mobile_createMessage.style.position = "absolute";
popup_mobile_createMessage.style.border = "solid";
popup_mobile_createMessage.style.borderRadius = "5px";
popup_mobile_createMessage.style.padding = "5px";
popup_mobile_createMessage.style.top = "90%"
popup_mobile_createMessage.style.left = "0px"
popup_mobile_createMessage.style.zIndex = "5";
popup_mobile_createMessage.style.width = "95%";
popup_mobile_createMessage.style.height = "20%";
popup_mobile_createMessage.style.backgroundColor = "#474b53";
popup_mobile_createMessage.innerHTML = 
"<button>cancel</button>&nbsp;<button>send</button><br>" +
"<br><input type='search' ></input><br>";

let defaultMarginBottom = el_chatNav.style.marginBottom;
let defaultHeight = el_chatNav.style.height;

popup_mobile_createMessage.getElementsByTagName('button')[0].style.width = "45%";
popup_mobile_createMessage.getElementsByTagName('button')[0].style.marginRight = "5%";
popup_mobile_createMessage.getElementsByTagName('button')[0].style.height = "25%";
popup_mobile_createMessage.getElementsByTagName('button')[0].style.fontSize = "4vw";
popup_mobile_createMessage.getElementsByTagName('button')[1].style.width = "45%";
popup_mobile_createMessage.getElementsByTagName('button')[1].style.height = "25%";
popup_mobile_createMessage.getElementsByTagName('button')[1].style.fontSize = "4vw";
popup_mobile_createMessage.getElementsByTagName('input')[0].style.fontSize = "4vw";
popup_mobile_createMessage.getElementsByTagName('input')[0].style.width = "80%";

popup_mobile_createMessage.getElementsByTagName('button')[0].onclick = function(){
	el_mobile_navhead.style.display = "block";
	el_chatTitle.style.display = "block";
	el_mobile_navIcon.style.display = "block";
	el_msgbox.style.display = "block";
	el_chatNav.style.marginBottom = defaultMarginBottom;
	el_chatNav.style.height = defaultHeight;
	document.body.removeChild(popup_mobile_createMessage);
}

popup_mobile_createMessage.getElementsByTagName('button')[1].onclick = function(){
	// do send the message
	sendMessage(popup_mobile_createMessage.getElementsByTagName('input')[0])
	el_mobile_navhead.style.display = "block";
	el_chatTitle.style.display = "block";
	el_mobile_navIcon.style.display = "block";
	el_msgbox.style.display = "block";
	el_chatNav.style.marginBottom = defaultMarginBottom;
	el_chatNav.style.height = defaultHeight;
	document.body.removeChild(popup_mobile_createMessage);
}

popup_mobile_createMessage.getElementsByTagName('input')[0].onsearch = function() {
	sendMessage(popup_mobile_createMessage.getElementsByTagName('input')[0])
	el_mobile_navhead.style.display = "block";
	el_chatTitle.style.display = "block";
	el_mobile_navIcon.style.display = "block";
	el_msgbox.style.display = "block";
	el_chatNav.style.marginBottom = defaultMarginBottom;
	el_chatNav.style.height = defaultHeight;
	document.body.removeChild(popup_mobile_createMessage);
}
// https://stackoverflow.com/questions/36430561/how-can-i-check-if-my-element-id-has-focus
function initPage(){

	if((/Android|webOS|iPhone|iPad|iPod|BlackBerry|IEMobile|Opera Mini/i.test(navigator.userAgent))
	){
		el_html.style.fontSize = "4vw";
		el_friendsNav.style.display = "none";
		el_roomsNav.style.display = "none";
		el_chatNav.style.marginLeft = "0%";
		el_chatNav.style.width = "100%";

     	el_msgbox.style.border = "none"
 	    el_msgbox.style.width = "100%";
 	    el_msgbox.style.marginLeft = "0%";
     	el_msg.style.width = "90%";
		el_msg.style.marginLeft = "5%";
		el_mobile_navhead.style.display = "block";

		el_msg.onclick = function() {
			el_msgbox.style.display = "none";
			el_chatTitle.style.display = "none";
			el_mobile_navhead.style.display = "none";
			el_mobile_navIcon.style.display = "none";
			document.body.appendChild(popup_mobile_createMessage);
			popup_mobile_createMessage.getElementsByTagName('input')[0].value = "";
			popup_mobile_createMessage.getElementsByTagName('input')[0].focus();
		}
	}
} 

initPage();

let mobile_nav_active = false;

window.addEventListener("resize", initPage);

el_mobile_navIcon.onclick = () => {
	if(!mobile_nav_active){
		el_mobile_nav.style.display = "block";
	}else{
		el_mobile_nav.style.display = "none";
	}

	mobile_nav_active = !mobile_nav_active;
}
