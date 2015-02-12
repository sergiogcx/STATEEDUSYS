var systemTimerInterval = 10 * 1000; // Every 10 seconds
var dialogDiv = "#dialog";
var dialogDivTitle = "#dialogtitle"
var dialogDivContent = "#dialogcontent";

// "Name:id:status", ...
var systemUsers = [];
/*
	CHAT SYSTEM jQUERY ENDPOINT
*/

var httpServer = "https://www.statecertify.com/cnss/welcome";
var processinglocation = "/cnss/javascript/processing.gif";
var deleteimage = "/cnss/javascript/delete.png"
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
var toggled = false;


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
var usersName = "#sessionusersname";
var divRightBox = "rightBox";
var rightBox = "#" + divRightBox;

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

function debug(msg) {
	console.log(msg);
	//$(debugBox).append('<p>'+msg+'</p>');
	//scrollDown();
}


function scrollDown() {
	if(activeSession>0) {
		elem = document.getElementById("ses_session_"+activeSession+"_messages");
		elem.scrollTop = elem.scrollHeight;
	}
}


function getSessionId() {
	return $(sessionid).val();
}

function getUsersName() {
	return $(usersName).val();
}


function triggerSubmit() {
	if(activeSession==0) {
		alert("No active session"); 
		return;
	}
	
	if($(messageBox).val().length > 0)
	{
		postSystemMessage($(messageBox).val());
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
  // optional: use 100% HTML5 mode where available
  // preferFlash: false,
  debugMode: false,
  
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

























var systemMessageDownloadQueue = [];
var systemMessageProcessQueue = [];
var systemPendingApproval = [];
var activeSession = 0;


function user(id,name,sesid, status,messages) {
	debug("Constructing user: " + id + "," + name + "," + status + "," + messages + "," + sesid );
	this.id=id;
	this.name=name;
	this.status=status;
	this.messages=messages
	this.sesid=sesid;
}

function messagepacket(sessionid, difference) {
	debug("Constructing messagepacket: " + sessionid + "," + difference );
	this.sessionid = sessionid;
	this.difference = difference;
}

function messageprocess (sessionid, name, message) {
	debug("Constructing messageprocess: " + sessionid + "," + name + "," + message );
	this.sessionid = sessionid;
	this.name = name;
	this.message = message;
}


// Official Internal Use
var sessions_data_packets = [];
var sessions_data_user_packets = [];
var sessions_data_user_data = "";
var adduser_auxstr = "";


function connectedSuccesfulMessage() {
	debug("System thread is running, you should now be connected.");
}

function initializeSystemTimer() {
	connectedSuccesfulMessage();
	timerVar = setInterval(function(){runSystemThread()}, systemTimerInterval);
}

function startSystemTimer() {
	debug("startSystemTimer()");
	stopTimer(); // clear it first
	initializeSystemTimer();
}

function runSystemThread() {
	debug("runSystemThread(): running thread");
	getSessionsStatus();
}

function getSystemUsersCount() {
	return systemUsers.length;
}

function getHighestID() {
	return getSystemUsersCount() == 0 ? 0 : systemUsers[0].id;
}

function addDialogContentName(id, name) {
	debug("addDialogContentName(): id:" + id + ", name: " + name);
	if(id != null & name != null) {
		$(dialogDivContent).append("<div id='dialog_name_"+id+"_div'><a id='dialog_name_"+id+"' class='dialog_link' href='javascript: activateUserFromDialog("+id+");'>Accept User("+id+") "+name+"</a></div>");
	}
}

function setDialogContent(content) {
	debug("setDialogContent(): content: " + content);
	if(content != null) {
		$(dialogDivContent).html(content);
	}
}

function showDialog() {
	debug("showDialog(): showing dialog");
	soundManager.play('session_pending');
	$(dialogDiv).show();
}

function closeDialog() {
	debug("closeDialog(): closing dialog");
	soundManager.stop('session_pending');
	$(dialogDiv).hide();
}

function addSystemUserContainer(id, name) {
	debug("addSystemUserContainer(): adding container");
	
	var template = "<div id='ses_session_{id}' class='ses_session_contentbox'>";

	template += 		"<div id='ses_session_{id}_titlebar' class='yellowbar'>"+
							"<img class='yellowusericon' src='/cnss/javascript/user.png' border='0' />"+
							"<a class='yellowtitle' href='javascript: systemUserTrigger({id});'>{name}</a>" +
							
							"<div class='yellowoptions'><a href='javascript: activateUser({id});'>Activate</a></div>" + 
							"<div class='yellowoptions'><a href='javascript: mailUser({id});'>Email</a></div>" + 
							"<div class='yellowoptions'><a href='javascript: openProfile({id});'>Open Profile</a></div>" + 
							"<div class='yellowoptions'><a href='javascript: deleteUser({id});'>Delete</a></div>" + 
							
						"</div>" +
						"<div id='ses_session_{id}_messages' class='userdarkbackground' style='display: none;'></div>";
						
	template += "</div>"; 
					
	template = template.replace(/{id}/g, id);
	template = template.replace(/{name}/g, name);
	
	$(contentBox).append(template);
}

function genButtonCodename(id, name) {
	debug("genButtonCodename(): id: " + id + ", name:" + name);
	var output = "ses_button_" + id + "_" + name.replace(/\s+/g, '');
	debug("genButtonCodename(): output: "+ output);
	return output;
}

function addSystemUserButton(id, name, status) {
	debug("addSystemUserButton(): id: " + id + ", name: " + name + ", status: " + status);
	var template = '<button class="ses_button {mode}" id="{codeid}" onclick="systemUserTrigger({usrid});">{username}</button>';
	var codenameid = genButtonCodename(id, name);
		// Game
		
		template = template.replace("{mode}", status == 0 ? "bpend" : "blist");
		template = template.replace("{codeid}",codenameid);
		template = template.replace("{codename}",codenameid);
		template = template.replace("{usrid}", id);
		template = template.replace("{username}", name);
	
	$(rightBox).append(template);
}

function addSystemUser(data) {
	debug("addSystemUser() Adding user : " + data);

	adduser_auxstr = data.split("|");
	systemUsers.push( new user(adduser_auxstr[0],
							   adduser_auxstr[1],
							   adduser_auxstr[2],
							   adduser_auxstr[3],
							   0 ) // <-- this should be always 0, download queue should adjust it
							);
							
	// Then make a download queue
	systemMessageDownloadQueue.push( new messagepacket(adduser_auxstr[0], 
														adduser_auxstr[4] ) // <-- now the queue will take care of the number of messages... usually just 1
														);
													
	addSystemUserButton(adduser_auxstr[0], 
						adduser_auxstr[1],
						adduser_auxstr[3]);
						
	addSystemUserContainer(adduser_auxstr[0], adduser_auxstr[1]);
 
	if(adduser_auxstr[3] == 0) {
		debug("addSystemUser(): we got an inactive user...");
		addDialogContentName(adduser_auxstr[0],  adduser_auxstr[1]);
	}
}

function deleteSystemUser(id) {
	// one at the time...
	debug("deleteSystemUser() deleting user id: " + id);
	for(var n = 0; n < systemUsers.length; n++) {
		debug(systemUsers[n].id + " == " + id);
		if(""+systemUsers[n].id == ""+id) {
			debug("Found a match!, deleting : " + systemUsers[n].name);
			
			systemUsers.splice(n,1);
			
			
			break; // stop the loop
		}
	}
	
	debug("deleteSystemUser() done deleting...");
	showUserStack();
}

function updateSystemUserMessageCount(id, amount) {
	// one at the time???
	debug("updateSystemUserMessageCount() updating user id: " + id + " by " + amount + " messages");
	for(var n = 0; n < systemUsers.length; n++) {
		debug(systemUsers[n].id + " == " + id);
		if(""+systemUsers[n].id == ""+id) {
			debug("updateSystemUserMessageCount() found a match, updating ("+systemUsers[n].messages+"): " + systemUsers[n].name);
			systemUsers[n].messages = parseInt(systemUsers[n].messages) + parseInt(amount);
			debug("updateSystemUserMessageCount(): Done! new amount: " + systemUsers[n].messages);
			break; // stop the loop
		}
	}
	
	debug("updateSystemUserMessageCount() done updating...");
	showUserStack();
}

function showUserStack() {
	debug("----- showUserStack(): here it comes! ...");
	for(var n = 0; n < systemUsers.length; n++) {
		debug(systemUsers[n].id + ">" + systemUsers[n].name + ">" + systemUsers[n].messages);
	}
	debug("----- showUserStack(): done! ...");
}


function getSessionsStatus() {
	debug("getSessionsStatus(): checking if we are have any sessions... active or not!");
	var url = httpServer + "/getsessionsstatus";
	
	debug("getSessionsStatus(): getting: " + url);
	
	$.get(url, function(data) {
		parseSessionsStatusData(data);
	})
	.done(function() { debug("getSessionsStatus() done"); runUserCheck();  })
	.fail(function() { debug("getSessionsStatus() failed"); });
}

function checkUserSessionPacketNewMessages(packet) {
	debug("checkUserSessionPacketNewMessages() running...");
	if(!packet) {
		debug("checkUserSessionPacketNewMessages() null packet provided, exiting...");
		return;
	}
	
	// 16|Sergio Garcia|123090|1|6
	var items = packet.split("|");
	var difference = 0;
	
	for(var n = 0; n < systemUsers.length; n++) {
		if(systemUsers[n].id == items[0]) {
			debug("checkUserSessionPacketNewMessages() user found internally..., packet has: " + items[4] + " msgs, internal user has: " + systemUsers[n].messages);
			difference = items[4] - systemUsers[n].messages;
			debug("checkUserSessionPacketNewMessages() difference: " + difference);
	
			if(difference > 0) {
				// Then make a download queue
				systemMessageDownloadQueue.push( new messagepacket( systemUsers[n].id  , 
																	difference ) );
			}
			
			return;
		}
	}
	
	debug("checkUserSessionPacketNewMessages() ID not found!");
}

function parseSessionsStatusData(data) {
	debug("parseSessionsStatusData(): data: " + data);
	var n = 0; // this will be usefyl
	var packetHighId = 0;
	var packetSessionsCount = 0;
	var difference = 0;
	var approvalHTML = "";
	
	debug("parseSessionsData(): populating data & queue...");
	// Split this -> '#'
	sessions_data_packets = data.split("#");
	
	// Then split -> '$'
	sessions_data_user_packets = sessions_data_packets[2].split("$");
	packetHighId = sessions_data_user_packets[0].substring(0, sessions_data_user_packets[0].indexOf('|'));
	packetSessionsCount = sessions_data_user_packets.length;
	
	debug("parseSessionsData() Received Packet Sessions Count: " + packetSessionsCount);
	debug("parseSessionsData() Reveived Packet Highest ID Count: " + packetHighId );
	
	debug("parseSessionsData() Internal Stack Sessions Count: " + getSystemUsersCount());
	debug("parseSessionsData() Internal Stack Highest ID: " + getHighestID());
	
	// First calculate any internal difference in messages count (per user)
	for(var i = 0; i < packetSessionsCount; i++) {
		checkUserSessionPacketNewMessages(sessions_data_user_packets[i]);
	}
	
	
	// Then check for new users
	if(getHighestID() != packetHighId) {
		debug("parseSessionsData() Highest session ID discrepancy: it is currently: " + getHighestID() + " should be: " + packetHighId);
		// First calculate the REAL difference
			// received elements in packet - elements
			if(getSystemUsersCount() != packetSessionsCount) {
					debug("parseSessionsData() Calculating REAL sessions count difference: it is currently: " + getSystemUsersCount() + " should be: " + packetSessionsCount);
					
					// First calculate the difference
					difference = (packetSessionsCount - getSystemUsersCount());
					debug("parseSessionsData(): The difference is: : " + difference );
					for(n = 0; n < difference; n++) {
						addSystemUser(sessions_data_user_packets[n]);
					}
					
					showDialog();
			} 
	} else {
		difference = (packetSessionsCount - getSystemUsersCount());
		debug("parseSessionsData(): No HighID difference, checking count difference: " + difference );
		// Make sure there is no difference
		if(difference > 0) {
			debug("parseSessionsData(): Dont stop loop until you find it!...");
			for(n = 0; n < sessions_data_user_packets.length; n++) {
				if(!systemUserExists(sessions_data_user_packets[n].split("|", 1))) {
					debug("parseSessionsData(): Found it! ('"+sessions_data_user_packets[n]+"'), now adding!...");
					addSystemUser(sessions_data_user_packets[n]);
				}
			}
			
		}
	}

	debug("parseSessionsData() Internal $ codings count: " + sessions_data_user_packets.length);
	
	// if there is a difference :: then re-populate
	showUserStack();
}

function runUserCheck() {
	debug('runUserCheck(): checking if there are any messages to download...');
	// 14:5|15:5
	var requestString = "";
	var packetTemp = new messagepacket(0,0);
	var n = 0;
	
	while(systemMessageDownloadQueue.length > 0) {
		packetTemp = systemMessageDownloadQueue.pop();
		debug("runUserCheck(): User id " + packetTemp.sessionid + ", has " + packetTemp.difference + " messages to be downloaded.");
		requestString += packetTemp.sessionid + ":" + packetTemp.difference;
		if(n<systemMessageDownloadQueue.length) requestString += "|";
	}
	
	downloadMessages(requestString);
}

function downloadMessages(requestString) {
	debug("downloadMessages(): generating message download packet: " + requestString);
	var url = httpServer + "/downloadOperatorMessages";
	// Post Message to Server
	$.post(url, {  request: requestString }  )
		.done(function(data) { 
				debug("downloadMessages(): Data Received: " + data);  
				populateDownlodProcess(data);   
				})
		.fail(function() { debug("downloadMessages(): Request could not be sent..."); });
}

function populateDownlodProcess(data) {
	debug("populateDownlodProcess(): data to populate: " + data)
	var messages = data.split("#");
	var ticket = "";
	var tokens = [];
	var tickets = [];
	var koolstuff = [];
	
	while(messages.length > 0) {
		ticket = messages.pop();
		tickets = ticket.split("*");
			
		debug("populateDownlodProcess() posting message in div container ... @ id: " + tickets[0] + ", ticket: " + ticket);
		for (var n = 1; n < tickets.length; n++) {
			debug("populateDownlodProcess() appending ticket["+n+"]: ("+tickets[n]+")... @ id: " + tickets[0]);
			appendMessageAtContainer(tickets[0], tickets[n]);
		}
	}
}


// jQuery message upload POST method
function postSystemMessage(message) {
	debug("postMessage(): posting message: " + message);
	var url = httpServer + "/postmessage";
	
	appendMessageAtContainer(activeSession, "System$"+message);
	
	// Post Message to Server
	$.post(url, {  from: getUsersName(), sessionid: activeSession,  message: ("" + message) }  )
		.done(function(data) { debug("postMessage(): Message Received: " + data);  })
		.fail(function() { debug("postMessage(): Message Could not be sent..."); });
}

function appendMessageAtContainer(id, message) {
	debug("appendMessageAtContainer(): " + id + ", " + message);
	var splitting = [];
	splitting = message.split("$");
	if(splitting.length > 1) {
		switch(splitting[0]) {
			case 'System': 
				$("#ses_session_"+id+"_messages").append('<div align="left"><div class="bubble me"><span class="tail">&nbsp;</span>System: '+splitting[1]+'</p></div></div>');
				break;
			default:
				$("#ses_session_"+id+"_messages").append('<div align="right"><div class="userbubble you"><span class="usertail">&nbsp;</span>'+splitting[0]+': '+splitting[1]+'</p></div></div>');
				break;
		}
	}
	updateSystemUserMessageCount(id, 1);
	
	soundManager.play('session_newmessage');
	scrollDown();
}

function systemUserTrigger(id) {
	debug("systemUserTrigger(): " + id);
	
	if(id == activeSession) {
		$("#ses_session_"+id+"_messages").toggle();
	} else {
		var tmp = activeSession;
		activeSession = id;
		$("#ses_session_"+tmp+"_messages").hide();
		$("#ses_session_"+activeSession+"_messages").show();
	}
}

function activateSystemUser(id) {
	debug("activateSystemUser() activating user in sytem...");
	for(var n = 0; n < systemUsers.length; n++) {
		if(systemUsers[n].id == id) {
			systemUsers[n].status = 1;
			var codename = genButtonCodename(systemUsers[n].id, systemUsers[n].name);
			$("#"+codename).attr("class", "ses_button blist");
			debug("activateSystemUser() done activating : " + codename);
			return;
		}
	}
	debug("activateSystemUser() ID not found!");
}

function checkUserActive(id) {
	// one at the time...
	debug("checkUserActive() checking if user is active: " + id);
	var buttonname = null;
	for(var n = 0; n < systemUsers.length; n++) {
		if(""+systemUsers[n].id == ""+id) {
			debug("checkUserActive() Found a match!, user id: "+id+", current status: " + systemUsers[n].status);
			return systemUsers[n].status == 1;
		}
	}
	
	debug("checkUserActive() could not find id:" + id);
	return false;
}

function setUserDialogLinkIdContent(sessionid, innerhtml) {
	debug("setUserDialogLinkIdContent(): sessionid: " + sessionid + ", innerhtml: " + innerhtml);
	var dialoglinkid = "#" + getUserDialogLinkId(sessionid);
	$(dialoglinkid).html(innerhtml);
}

function getUserDialogLinkId(sessionid) {
	return "dialog_name_" + sessionid;
}

function animateDialogUserLinkGetImageId(sessionid) {
	return getUserDialogLinkId(sessionid) + "_image";
}

function animateDialogUserLinkError(sessionid) { 
	debug("animateDialogUserLinkError() sessionid: " + sessionid);
	var dlink = "#" + getUserDialogLinkId(sessionid);
	var processingImageHTML = "<img id='"+ animateDialogUserLinkGetImageId(sessionid) +"' src='"+deleteimage+"' border='0'/>";
	var innerHTML = processingImageHTML + $(dlink).html();
	debug("animateDialogUserLinkError() animating error: " + dlink);
	setUserDialogLinkIdContent(sessionid, innerHTML);
}

function animateDialogUserLinkPending(sessionid) {
	debug("animateDialogUserLinkPending() sessionid: " + sessionid);
	var dlink = "#" + getUserDialogLinkId(sessionid);
	var processingImageHTML = "<img id='"+ animateDialogUserLinkGetImageId(sessionid) +"' src='"+processinglocation+"' border='0'/>";
	var innerHTML = processingImageHTML + $(dlink).html();
	debug("animateDialogUserLinkPending() animating pending: " + dlink);
	setUserDialogLinkIdContent(sessionid, innerHTML);
}

function animateDialogUserLinkPendingClearImage(sessionid) {
	debug("animateDialogUserLinkClear() sessionid: " + sessionid);
	var imgInstance = "#" + animateDialogUserLinkGetImageId(sessionid);
	$(imgInstance).remove();
}

function animateDialogUserLinkClear(sessionid) {
	debug("animateDialogUserLinkClear() sessionid: " + sessionid);
	animateDialogUserLinkPendingClearImage(sessionid);
}

function animateDialogUserLinkDelete(sessionid) { 
	var dialoglinkid = "#" + getUserDialogLinkId(sessionid) + "_div";
	debug("animateDialogUserLinkDelete(): animating: " + dialoglinkid);
	
	 $(dialoglinkid).animate({
		opacity: 0.0,
		left: '+=50',
		height: 'toggle'
	  }, 1500, function() {
		debug("animateDialogUserLinkDelete(): animation complete, removing...");
		$(dialoglinkid).remove();
		debug("animateDialogUserLinkDelete(): done removing...");
	  });
}

function activateUserFromDialog(sessionid) {
	// Activating From Dialog
	debug("activateUserFromDialog() sessionid: " + sessionid);
	var url = httpServer + "/activateUser";
	
	if(checkUserActive(sessionid)) {
		alert('User already Active');
		return;
	}
	
	animateDialogUserLinkPending(sessionid);
	
	// Post Message to Server
	$.post(url, {  id: sessionid }  )
		.done(function(data) { 
				debug("activateUserFromDialog(): Data Received: " + data);  
				if(data=="success") {
					activateSystemUser(sessionid);
					animateDialogUserLinkDelete(sessionid);
				}
				})
		.fail(function() { 
			debug("activateUserFromDialog(): Activation request failed...");
			animateDialogUserLinkClear(sessionid);
			});
		
}

function activateUser(sessionid) {
	// activateUserForChatting
	debug("activateUser(): activating user... " + sessionid);
	var url = httpServer + "/activateUser";
	
	if(checkUserActive(sessionid)) {
		alert('User already Active');
		return;
	}
	
	// Post Message to Server
	$.post(url, {  id: sessionid }  )
		.done(function(data) { 
				debug("activateUser(): Data Received: " + data);  
				if(data=="success") activateSystemUser(sessionid);
				})
		.fail(function() { debug("activateUser(): Activation request could not be sent..."); });

}

function deactivateUser(id) {
	// opposite
}

function deleteUser(id) {
	debug("deleteUser(): deleting user by id: " + id);
	var url = httpServer + "/deletesession?sessionid=" + id;
	
	// Cancel if not ready...
	if(!confirm('Delete user session? (Do not want to email first?)')) return;

	
	debug("deleteUser(): deletion url: " + url);
	
	$.get(url, function(data) {
		if(data == "deleted") {
			debug("deleteUser(): deleted in database & cookie...");
			
			debug("deleteUser(): Clear User 'Content box' From Left Panel");
			removeUserContentBox(id);
			debug("deleteUser(): Clear User 'button' From Right Panel");
			removeUserButton(id);
			debug("deleteUser(): Clear User From Internal Queue");
			deleteSystemUser(id);
			
		} else {
			debug("deleteUser(): error: " + data);
		}
	})
	.done(function() { debug("deleteUser(): done deleting"); })
	.fail(function() { debug("deleteUser(): delete failed"); });
}

function removeUserContentBox(id) {
	var container = "ses_session_"+id;
	debug("removeUserContentBox(): removing content box: " + container);
	$("#"+container).remove();
}

function removeUserButton(id) {
	// one at the time...
	debug("removeUserButton() removing user button: " + id);
	var buttonname = null;
	for(var n = 0; n < systemUsers.length; n++) {
		debug(systemUsers[n].id + " == " + id);
		if(""+systemUsers[n].id == ""+id) {
			debug("Found a match!, deleting : " + systemUsers[n].name);
			buttonname = genButtonCodename(id, systemUsers[n].name);
			debug("removeUserContentBox(): removing content box: " + buttonname);
			$("#"+buttonname).remove();
			
			return true;
		}
	}
	
	debug("removeUserContentBox() could not find id:" + id + ", button name: " + buttonname);
	return false;
}

function systemUserExists(id) {
	debug("systemUserExists() checking if system user exists in queue: " + id);
	for(var n = 0; n < systemUsers.length; n++) {
		debug(systemUsers[n].id + " == " + id);
		if(""+systemUsers[n].id == ""+id) {
			debug("systemUserExists(): Found a match!, returning true!");
			return true;
		}
	}
	debug("systemUserExists() done searching... nothing found for " + id + "?");
	return false;
}

function openProfile(id) {
	alert('openProfile(): Not implemented yet');
}

function mailUser(id) {
	alert('mailUser(): Not implemented yet');
}


function toggleSwitch() {
	debug("toggleSwitch(): toggling system timer: ");
	var url = httpServer + "/checkavailable"
	
	if(toggled)  {
		if(!readyToToggleOff()) {
			alert("You must close every conversation before toggling off.");
			debug("toggleSwitch(): Cannot toggle off, one or more threads running/active.");
			return;
		}
	}
	
	$.get(url, function(data) {
		if(data == "available") {
			debug("toggleSwitch(): turning ON...");
			toggleOn();
		} else {
			debug("toggleSwitch(): turning OFF...");
			toggleOff();
			
		}
		debug(data);
	})
	.done(function() { debug("toggleSwitch(): done"); })
	.fail(function() { debug("toggleSwitch(): failed"); });
}

function toggleImageContent(mode) {
	if(mode) { // On
		$("#togglex").attr("src","/cnss/javascript/on.png");
		toggled = true;
	} else { // Off
		$("#togglex").attr("src","/cnss/javascript/off.png");
		toggled = false;
	}
}

function toggleOn() {
	startSystemTimer();
	toggleImageContent(true);
}

function toggleOff() {
	stopTimer();
	toggleImageContent(false)
}

function readyToToggleOff() {
	return systemUsers.length == 0;
}
