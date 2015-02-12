/*
	CHAT SYSTEM jQUERY ENDPOINT
*/

var httpServer = "https://www.statecertify.com/cnss/welcome";
var contentIntervalCheck = 10 * 1000; // Every 10 Seconds
var liveIntervalCheck = 30 * 1000;	// Every 30 Seconds
var toggleSound = false;
var timerVar = null;
var localMessageCount = 0;
var liveCheckCycles = 0;
var liveCheckApologyCycles = 2; 	// Every minute
var liveCheckApologiesCount = 0;
var liveCheckApologiesCountLimit = 3;	// Leave Message at what attempt?
var systemLive = false;
var recoveryMode = false;
var debugmode = false;


// Content Box
var divContentBox = "contentBox";
var contentBox = "#" + divContentBox;
var divDebugBox = "debugBox";
var debugBox = "#" + divDebugBox;
var messageBox = "#message";
var messageDifference = "#sessionmsgd";
var http_status_box = "#http_status_box";
var http_status_waiting = "http_status_waiting";
var http_status_reading = "http_status_reading";
var sessionid = "#sessionid";
var sessionkey = "#sessionkey";
var usersName = "#sessionusersname";
var divRightBox = "rightBox";
var rightBox = "#" + divRightBox;

function debug(msg) {
	if(debugmode) {
		console.log(msg);
	} 
}

function initializeDebugMode(mode) {
	debug("initializeDebugMode(): debug is now: " + (mode ? "true" : "false" ));
	debugmode = mode;
}

function initializeRecovery() {
	debug("Initializing In Recovery Mode");
	recoveryMode = true;
	setSystemLive(true);
	getNumberOfMessages();
}

function initializeNormal() {
	debug("Initializing In Normal Mode");
	startTimer();
	getNumberOfMessages();
}

function updateLiveCheckCycles(num) {
	var was = liveCheckCycles;
	if(num==0) { liveCheckCycles = num; liveCheckApologiesCount++; }
	liveCheckCycles += num;
	debug("liveCheckCycles was: " + was + ", is now: " + liveCheckCycles)
	checkLeaveMessage();
}

function updateTimer() {
	var d=new Date();
	var t=d.toLocaleTimeString();
	document.getElementById("timer").innerHTML=t;
}

function updateMsgDif(number) {
	debug("updateMsgDif: " + number);
	var num = 0;
	if(number) num=number;
	
	$(messageDifference).val(num);
	document.getElementById("msgdiff").innerHTML=""+num;
}

function clearCurrentDifferences() {
	updateMsgDif(0);
}

function updateLocalMessagesCount(bynum) {
	
	debug("updateLocalMessagesCount: localMessageCount: " + localMessageCount + " + " + bynum);
	localMessageCount += parseInt(bynum);
	debug("updateLocalMessagesCount: localMessageCount is now: " + localMessageCount );
}

function recoverMessageThread() {
	
}

// This function will contact the server to check
// if there are any messages to be posted
function checkPendingMessages() {
	debug("checking pending messages ...");
	getNumberOfMessages();
	updateTimer();
	scrollDown();
}

function scrollDown() {
	// Scroll down!
	var elem = document.getElementById(divContentBox);
	elem.scrollTop = elem.scrollHeight;
	
	elem = document.getElementById(divDebugBox);
	elem.scrollTop = elem.scrollHeight;
}

function checkLive() {
	debug("checkLive(): checking if we are live!");
	if(systemLive) {
		debug("checkLive(): we are already live, ignoring this call!");
		return;
	}
	
	var url = httpServer + "/checklive?sessionid="+getSessionId();
	
	debug("checkLive(): getting: " + url);
	
	updateLiveCheckCycles(1);
	
	
	$.get(url, function(data) {
		if(data == "1") {
			writeSystemMessage("Support system is Live! " + getUsersName() + ", you are talking to Admin!");
			setSystemLive(true);
		}
		else {
			if(liveCheckCycles == liveCheckApologyCycles) {
				writeSystemMessage("Thank you for your patience, one of our operators will be with you shortly.");
				updateLiveCheckCycles(0);
			}
		}
	})
	.done(function() { debug("done"); })
	.fail(function() { writeSystemMessage("Could not connect to the server. Please wait a few seconds or check your connection and try again."); });
}

function setSystemLive(value) {
	debug("setSystemLive?: " + ( value? "We are Live!" : "Not yet..."));
	systemLive = value; // true or false
	if(systemLive) {
		stopTimer();
		restartTimer();
		enableTextArea();
	}
}

function checkLeaveMessage() {
	if(liveCheckApologiesCount == liveCheckApologiesCountLimit)
	writeSystemMessage("It appears none of our operators are available as of this moment. Would you rather leave a message or call?");
}

function getSessionId() {
	return $(sessionid).val();
}

function getSessionKey() {
	return $(sessionkey).val();
}

function getMessages() {
	debug("getting messages");
	var msgDiffrx = $(messageDifference).val();
	var url = "";
	
	if(msgDiffrx==0) {
		debug("difference == 0, ignoring request");
		return;
	}
	
	debug("downloading " + msgDiffrx + " messages ...");
	if(recoveryMode == true) {
		debug("It appears to be a recovery....");
		url = httpServer + "/getmessages?sessionid="+getSessionId()+"&difference="+msgDiffrx+"&mode=recovery";
		recoveryMode = false; // Switch off
	}
	else {
		debug("It appears to be a normal run....");
		url = httpServer + "/getmessages?sessionid="+getSessionId()+"&difference="+msgDiffrx;
	}
	debug("getting: " + url);
	
	$.get(url, function(data) {
		receiveDbMessage(data);
	})
	.done(function() { debug("done"); updateLocalMessagesCount(msgDiffrx); clearCurrentDifferences(); })
	.fail(function() { debug("failed"); });
}

function getNumberOfMessages() {
	$(http_status_box).attr("class", http_status_reading);
	
	debug("getting number of messages");
	var url = httpServer + "/getnumberofmessages?sessionid="+getSessionId();
	var difference = 0;
	debug("getting: " + url);
	
	$.get(url, function(data) {
		debug("data: " + data);
		
		difference = parseInt(data) - localMessageCount;
		
		updateMsgDif(difference);
		
		if(difference == 0)
			debug("Same");
		else {
			debug("Difference: " + difference);
			getMessages();
		}
	})
	.done(function() {	debug("done"); 
						$(http_status_box).attr("class", http_status_waiting);
						})			
	.fail(function() { debug("failed"); });
}

function getUsersName() {
	return $(usersName).val();
}

// Parse Data From Server
function receiveDbMessage(data) {
	var messages = data.split("~");
	var n = 0;
	for(n = 0; n < messages.length-1; n++) {
		var message = messages[n].split("|");
		if(message[0] == "System")
			writeSystemMessage(message[1]);
		else
			writeUserMessage(message[1]);
	}
}

// jQuery CSS Div class Balloon System
function writeSystemMessage(msg) {
	if(msg != "ATTENTION: System: User has left.") {
		$(contentBox).append('<div align="left"><div class="bubble me"><span class="tail">&nbsp;</span>'+msg+'</p></div></div>');
		scrollDown();
	}
}

// jQuery CSS Div class Balloon User
function writeUserMessage(msg) {
	$(contentBox).append('<div align="right"><div class="userbubble you"><span class="usertail">&nbsp;</span>'+msg+'</p></div></div>');
	scrollDown();
}

// jQuery message upload POST method
function postMessage(message) {
	debug("postMessage(): posting message: " + message);
	var url = httpServer + "/postmessage";
	
	writeUserMessage(message);
	
	// Post Message to Server
	$.post(url, {  from: getUsersName(), sessionid: getSessionId(),  message: ("" + message) }  )
		.done(function(data) { debug("Message Received: " + data);  updateLocalMessagesCount(1);   })
		.fail(function() { debug("Message Could not be sent..."); });
}

function enableTextArea() {
	$(messageBox).val("");
	$(messageBox).attr("disabled", false);
}

function triggerSubmit() {
	if(!systemLive) return;
	if($(messageBox).val().length > 0)
	{
		postMessage($(messageBox).val());
		clearMessage();
	}
}

function clearMessage() {
	$(messageBox).val("");
}

$(document).keypress(function(e) {
    if(e.which == 13) {
		if($(messageBox).val().length > 0)
			triggerSubmit();
		else
			alert('Please write something!');
    }
	
	if (e.charCode!=0) {
        var regex = new RegExp("^[a-zA-Z0-9 _-]|[@.,;!?]+$");
        var key = String.fromCharCode(!e.charCode ? e.which : e.charCode);
        if (!regex.test(key)) {
            e.preventDefault();
            return false;
        }
    }
});

function startTimer() {
	timerVar = setInterval(function(){checkLive()}, liveIntervalCheck);
}

function restartTimer() {
	timerVar = setInterval(function(){checkPendingMessages()}, contentIntervalCheck);
}

function stopTimer() {
	debug("stopTimer(): clearing timer interval");
	clearInterval(timerVar);
}


soundManager.setup({
  url: '/cnss/javascript/_swf',
  debugMode: false,
  // optional: use 100% HTML5 mode where available
  // preferFlash: false,
  onready: function() {
    var soundSystemInitialized = soundManager.createSound({
      id: 'aSound',
      url: '/cnss/javascript/mp3/Connect.wav'
    }); 
	
	
	soundManager.createSound({
		id: 'session_started',
		url: '/cnss/javascript/mp3/StartTransfer.wav'
	});

	soundManager.createSound({
		id: 'session_newmessage',
		url: '/cnss/javascript/mp3/ContactOn.wav',
		autoLoad: true
	});

	soundManager.createSound({
		id: 'session_ended',
		url: '/cnss/javascript/mp3/Disconnect.wav',
		autoLoad: true
	});

	soundManager.createSound({
		id: 'session_pending',
		url: '/cnss/javascript/mp3/Dialing.mp3',
		autoLoad: true,
		volume:50
	});

	
    soundSystemInitialized.play();
  },
  ontimeout: function() {
    // Hrmm, SM2 could not start. Missing SWF? Flash blocked? Show an error, etc.?
	alert("Could not set up sound!");
  }
});


function checkClose() {
	notificateExit();
    return "Are you sure you want to leave?";
}

function notificateExit() {
	var url = httpServer + "/handleuserexit?sessionid="+getSessionId()+"&tag="+getSessionKey();
	
	$.get(url, function(data) {
		debug("notificateExit(): data received: " + data);
	})
	.done(function() { debug("notificateExit(): done"); })
	.fail(function() { debug("notificateExit(): failed"); });
}

function exitConversation() {
	notificateExit();
	window.close();
}

function quitConversation(mode) {
	if(mode >= 0) {
		if (confirm('Do you want to be sent an email with a transcript?')) {
			emailConversation();
		}
	}
	if(mode==1) {
		if(confirm("Do you want to exit this conversation?")) {
			exitConversation();
		}	
	}
}

function emailConversation() {
	var url = httpServer + "/mailSession?sessionid="+getSessionId()+"&key="+getSessionKey();
	
	$.get(url, function(data) {
		if(data == "ses_sent")
			alert("Email requested and sent!");
	})
	.done(function() { 
		
		debug("emailConversation(): done"); 
	
		if(confirm("Do you want to exit this conversation?")) {
			exitConversation();
		}	
	
	})
	.fail(function() { debug("emailConversation(): failed"); });
}

$(window).bind('beforeunload', checkClose);
