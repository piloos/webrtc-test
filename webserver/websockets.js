function debug(message) {
	console.log(message);
}

function sendMessage(msg) {
	console.log("Send");
	if ( websocket != null )
	{
		websocket.send( msg );
		console.log( "OUT:", '"'+msg+'"' );
	} else {
		console.log("No socket available to send message");
	}
}

var wsUri =  "wss://" + window.location.hostname + ":1234"; // "wss://localhost:1234";
var websocket = null;

function initWebSocket() {
	try {
		if (typeof MozWebSocket == 'function')
			WebSocket = MozWebSocket;
		if ( websocket && websocket.readyState == 1 )
			websocket.close();
		websocket = new WebSocket( wsUri );
		websocket.onopen = function (evt) {
			debug("CONNECTED");
			//startWebRTC(true);
			//onSocketConnectedWebRTC();

			//sendMessage("hello");
		};
		websocket.onclose = function (evt) {
			debug("DISCONNECTED");
		};
		websocket.onmessage = function (evt) {
			console.log( "Message received :", evt.data );
			onSocketReceiveWebRTC(evt.data);
			debug( evt.data );
		};
		websocket.onerror = function (evt) {
			debug('ERROR: ' + evt.data);
		};
	} catch (exception) {
		debug('ERROR: ' + exception);
	}
}

function stopWebSocket() {
	if (websocket)
		websocket.close();
}

function checkSocket() {
	if (websocket != null) {
		var stateStr;
		switch (websocket.readyState) {
			case 0: {
				stateStr = "CONNECTING";
				break;
			}
			case 1: {
				stateStr = "OPEN";
				break;
			}
			case 2: {
				stateStr = "CLOSING";
				break;
			}
			case 3: {
				stateStr = "CLOSED";
				break;
			}
			default: {
				stateStr = "UNKNOW";
				break;
			}
		}
		debug("WebSocket state = " + websocket.readyState + " ( " + stateStr + " )");
	} else {
		debug("WebSocket is null");
	}
}
