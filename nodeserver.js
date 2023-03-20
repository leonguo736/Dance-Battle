var uuid = require('uuid-random');
const WebSocket = require('ws');

const wss = new WebSocket.WebSocketServer({port:8080}, ()=> {
	console.log('server started');
});

// Object that stores player data 
const playersData = {
	"type" : "playerData"
}

// variable used to differentiate host, player 1 and player 2
var connectionCount = 1;

// Object that stores pose information
class Pose {
    constructor(beat, leftArm, rightArm) { 
		this.beat = beat;
        this.leftArm = leftArm;
        this.rightArm = rightArm;
    }
}
var pose0 = new Pose(1, 100, 200);
var pose1 = new Pose(2, 400, 90);
var song = [pose0, pose1];
var songBytes = "262.2, 294.8, 327.5, 349.3, 393.0, 436.7, 491.2, 524.0, 589.5";

/* arrays to store poses */
var beatArray_atk = [];
var leftArray_atk = [];
var rightArray_atk = [];
var beatArray_def = [0, 1, 2, 3, 4, 5];
var leftArray_def = [10, 20, 30, 40, 50];
var rightArray_def = [5, 4, 3, 2, 1];
var score = 0;

// Websocket function that managages connection with clients
wss.on('connection', function connection(client){

	//Create Unique User ID for player
	// client.id = uuid();
	switch(connectionCount) {
		case 1:
			client.id = "Host";
			break;
		case 2:
			client.id = "PlayerA";
			break;
		case 3:
			client.id = "PlayerB";
			break;
		default:
			client.id = "extra";
	}
	console.log(`${client.id} Has Connected!`);
	
	if (client.id == "Host") {
		client.send(songBytes);
	}

	//Method retrieves message from client
	client.on('message', (data) => {
		console.log("Player Message");
		console.log(`received: ${data}`);
		
		// add if statement later to only read from attacker. also rename arrays to differentiate between atk and def. 
		// then compare atk vs def to get score
		// if (client.id == "PlayerA") {
		// 	var dataArray_atk = data.split(",");
		// 	for (var i = 0; i < dataArray_atk.length/3; i++) {
		// 		beatArray_atk.push(dataArray_atk[i]);
		// 		leftArray_atk.push(dataArray_atk[i+1]);
		// 		rightArray_atk.push(dataArray_atk[i+2]);
		// 	}
		// }
		// else if (client.id == "PlayerB") {
		// 	var dataArray_def = data.split(",");
		// 	for (var i = 0; i < dataArray_def.length/3; i++) {
		// 		beatArray_def.push(dataArray_def[i]);
		// 		leftArray_def.push(dataArray_def[i+1]);
		// 		rightArray_def.push(dataArray_def[i+2]);
		// 	}
		// }
		const dataArray_atk = data.toString().split(",");
		for (var i = 0; i < dataArray_atk.length/3; i++) {
			beatArray_atk.push(dataArray_atk[i]);
			leftArray_atk.push(dataArray_atk[i+1]);
			rightArray_atk.push(dataArray_atk[i+2]);
			console.log("pushed: " + dataArray_atk[i+2]);
		}
		for (var i = 0; i < dataArray_atk.length/3; i++) {
			// console.log(Math.parseFloat(leftArray_atk[i]));
			// console.log("calculation:" + (Math.abs(parseFloat(leftArray_atk[i]) - leftArray_def[i])));
			// score += 25 - Math.abs(parseFloat(leftArray_atk[i]) - leftArray_def[i]);
			// score += 25 - Math.abs(parseFloat(rightArray_atk[i]) - rightArray_def[i]);
		}
		// console.log("score: " + score);
		if (client.id == "Host") {
			client.send(data);
		}
		if (data.toString() == "score") {
			client.send(score.toString());
		}
	});

	//Method notifies when client disconnects
	client.on('close', () => {
		console.log('This Connection Closed!')
		console.log("Removing Client: " + client.id)
	});

});

wss.on('listening', () => {
	console.log('listening on 8080')
});