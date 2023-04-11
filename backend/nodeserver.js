var uuid = require('uuid-random');
const WebSocket = require('ws');
const mongoose = require("mongoose");
var mongo = require("mongodb");
var Database = require("./Database1.js");

const wss = new WebSocket.WebSocketServer({port:8080}, ()=> {
	console.log('server started');
});
/**
 * IGNORE THIS SECTION OF COMMENTED CODE. 
 */
// var db = new Database("mongodb://127.0.0.1:27017", "cpen391");
// var messages = {};
// db.getRooms().then((result) => {
// 	console.log("werew");
// 	console.log(result);
// 	for (var i = 0; i < result.length; i++) {
// 		messages[result[i]._id] = [];
// 	}
// });

// const uri =
//   "mongodb+srv://leonguo736:12345@cluster0.jnwwabb.mongodb.net/?retryWrites=true&w=majority";

// async function connect() {
//   try {
//     await mongoose.connect(uri);
//     console.log("Connected to MongoDB");
// 	console.log(db);
//   } catch (error) {
//     console.error(error);
//   }
// }

// var db = connect();

// db.collection("collection1").insertOne(pose0, function(err, result) {
// 	console.log("item inserted");
// });


// Object that stores player data 
const playersData = {
	"type" : "playerData"
}

// connection count does not decrement right now
var connectionCount = 0;
/* set to 1 if connected, 0 if not */
var atkConnected = 0; 
var defConnected = 0;
var hostConnected = 0;
var expected_data = "default";

// Object that stores pose information
// 5 points --> 2 angles
class Pose {
    constructor(beat, leftArm, rightArm) { 
		this.beat = beat;
        this.leftArm = leftArm;
        this.rightArm = rightArm;
    }
}

class Point {
	constructor(x, y) {
		this.x = x;
		this.y = y;
	}
}
var pose0 = new Pose(1, 100, 200);
var pose1 = new Pose(2, 400, 90);
var song = [pose0, pose1];
var songBytes = "262.2, 294.8, 327.5, 349.3, 393.0, 436.7, 491.2, 524.0, 589.5";
var testString = ""

/* arrays to store poses */
var beatArray_atk = [];
var leftArray_atk = [];
var rightArray_atk = [];
var beatArray_def = [0, 1, 2, 3, 4, 5];
var leftArray_def = [10, 20, 30, 40, 50];
var rightArray_def = [5, 4, 3, 2, 1];
var attackerPoses = []
var receivedFrom = "";
// var deviceID

// Websocket function that managages connection with clients
wss.on('connection', function connection(client) {
	// Create Unique User ID for player
	// client.id = uuid();
	// client.id = connectionCount;
	// connectionCount++;
	// setTimeout(() =>{
	// 	for (client of wss.clients.values()) {
	// 		client.send("you are connected");
	// 		console.log("SENT TEST TO CLIENT: " + client.id);
	// 	}
	// }, 1000);
	// client.send("l" + atkConnected + " " + defConnected);
	
	//Method retrieves message from client
	client.on('message', (data) => {
		console.log("Player Message");
		console.log(`received: ${data}`);
		try {
			var parsedData = JSON.parse(data);
			console.log("parsed: " + parsedData);
		} catch (e) {
			console.log("Server received incorrectly formatted JSON string");
		}

		/* For lobby init. Reponds to the Host with info regarding to connected players */
		if (parsedData.command == "lobbyInit") {
			console.log("entered init");
			client.send("l" + atkConnected + " " + defConnected);
		}
		else if (parsedData.command == "setType") {
			console.log("entered set type");
			if (parsedData.identifier == "host") {
				hostConnected = 1;
				client.id = "host";
				console.log("host connected");

				client.send("l" + atkConnected + " " + defConnected);
			}
			else if (parsedData.identifier == "camera") {
				if (atkConnected == 0) {
					atkConnected = 1;
					client.id = "attacker";
					var data = "id,1";
					console.log("atk connected");
					client.send(data);
					
					for (de1 of wss.clients.values()) {
						// console.log("client id: " + client.id);
						if (de1.id == "host") {
							console.log("sending data to host");
							de1.send("l" + atkConnected + " " + defConnected);
						}
					}
				}
				else if (defConnected == 0) {
					defConnected = 1;
					client.id = "defender";
					var data = "id,2";
					console.log("def connected");
					client.send(data);

					for (de1 of wss.clients.values()) {
						// console.log("client id: " + client.id);
						if (de1.id == "host") {
							console.log("sending data to host");
							de1.send("l" + atkConnected + " " + defConnected);
						}
					}
				}
				else {
					console.log("extra client connected");
					var data = "extra client";
				}
		
				// setTimeout(() =>{
				// 	client.send(data);
				// }, 1000);
			}
		}
		else if (parsedData.command == "captureAttacker") {
			console.log("sending signal to attacker");
			for (de1 of wss.clients.values()) {
				if (de1.id == "attacker") {
					console.log("msg to attacker");

					/* work around for client id bug */
					console.log("expected (180): " + expected_data);
					expected_data = "attacker";
					// if (expected_data == "default") {
					// 	expected_data = "attacker";
					// }
					// else {
					// 	console.log("tried to send signal to attacker while awaiting data");
					// }
					/* end of work around */

					de1.send("cap," + parsedData.poseID);
				}
			}
		}
		else if (parsedData.command == "captureDefender") {
			console.log("sending signal to defender");
			for (de1 of wss.clients.values()) {
				if (de1.id == "defender") {
					console.log("msg to defender");

					/* work around for client id bug */
					console.log("expected (200): " + expected_data);
					expected_data = "defender";
					// if (expected_data == "default") {
					// 	expected_data = "defender";
					// }
					// else {
					// 	console.log("tried to send signal to defender while awaiting data");
					// }
					/* end of work around */
					// for (de2 of wss.clients.values()) {
					// 	if (de2.id == "host") {
					// 		expected_data = "default";
					// 		// var msg = "s" + poseID + " " + score;
					// 		var msg = "s1" + " " + "100";
					// 		de2.send(msg);
					// 	}
					// 	else if (de2.id == "defender") {
					// 		de2.send("cap," + parsedData.poseID);
					// 	}
					// }
					de1.send("cap," + parsedData.poseID);
				}
			}
		}
		else if (parsedData.command == "poseData") { // for handling the other 99% of messages
			console.log("reading pose data");
			
			console.log(parsedData.median);
			/**
			 * Assumes JSON has 3 fields: "command", "median" and "beat". Also assumes only one pose is sent per message.
			 * 
			 * Function takes numbers from "median" array and uses them to create 5 Point objects. 
			 * The points are passed through find_angle() which computes and angle in radians (more info on implementation at bottom).
			 * A pose object is then created using 2 angles and a beat, which is then passed to the host as a JSON string.
			 * Camera sets top left of grid is [0,0], so invert y-value for proper angle calculation
			 */

			/* set up the 5 points: CENTER IS CEHST, ONE POINT IS ALWAYS ON X AXIS (5, 0) */ 
			// var chest = new Point(parsedData.median[0][0], parsedData.median[0][1]);
			// var leftArm = new Point(parsedData.median[1][0], parsedData.median[1][1]);
			// var leftElbow = new Point(parsedData.median[2][0], parsedData.median[2][1]);
			// var rightArm = new Point(parsedData.median[3][0], parsedData.median[3][1]);
			// var rightElbow = new Point(parsedData.median[4][0], parsedData.median[4][1]);
			// var leftLeg = new Point(parsedData.median[5][0], parsedData.median[5][1]);
			// var leftKnee = new Point(parsedData.median[6][0], parsedData.median[6][1]);
			// var rightLeg = new Point(parsedData.median[7][0], parsedData.median[7][1]);
			// var rightKnee = new Point(parsedData.median[8][0], parsedData.median[8][1]);
			// var pelvis = new Point(parsedData.median[9][0], parsedData.median[9][1]);
			var chest = new Point(parsedData.median[0][0], -1 * parsedData.median[0][1]);
			var leftArm = new Point(parsedData.median[1][0], -1 * parsedData.median[1][1]);
			var rightArm = new Point(parsedData.median[2][0], -1 * parsedData.median[2][1]);
			var pelvis = new Point(parsedData.median[3][0], -1 * parsedData.median[3][1]);
			var leftLeg = new Point(parsedData.median[4][0], -1 * parsedData.median[4][1]);
			var rightLeg = new Point(parsedData.median[5][0], -1 * parsedData.median[5][1]);
			var tummy = new Point(chest.x, chest.y-10);
			var xaxis = new Point(parsedData.median[0][0] + 25, parsedData.median[0][1]);

			/* calculate the 2 angles and store data in a Pose object */
			var poseID = parsedData.poseId; // temporary placeholder. This should be a field in JSON string
			// var leftarm_angle = find_angle(leftArm, chest, leftElbow);
			// var rightarm_angle = find_angle(rightArm, chest, rightElbow);
			var leftarm_angle = find_angle(leftArm, xaxis, chest);
			var rightarm_angle = find_angle(rightArm, xaxis, chest);
			
			// Start Kerry
			function getAngle(a, b) {
				const deltaX = b.x - a.x;
				const deltaY = b.y - a.y;
				return Math.atan2(deltaY, deltaX);
			}

			function createPoint(x, y) {
				return new Point(x, 480 - y);
			}

			var mid = createPoint(chest.x, chest.y); // median[0]
			var left = createPoint(leftArm.x, leftArm.y); // median[1]
			var right = createPoint(rightArm.x, rightArm.y); // median[2]
			var pelvis = createPoint(pelvis.x, pelvis.y); // median[3]
			var leftLeg = createPoint(leftLeg.x, leftLeg.y); // median[4]
			var rightLeg = createPoint(rightLeg.x, rightLeg.y); // median[5]

			leftarm_angle = getAngle(mid, left); // give to alex, counter-cw for alex
			rightarm_angle = getAngle(mid, right); // give to alex, counter-cw for alex
			leftleg_angle = getAngle(pelvis, leftLeg); // give to alex, counter-cw for alex
			rightleg_angle = getAngle(pelvis, rightLeg); // give to alex, counter-cw for alex

			function toDegree(r) {
				return r * (180 / Math.PI);
			}

			console.log("leftarm_angle: " + toDegree(leftarm_angle) + " rightarm_angle: " + toDegree(rightarm_angle) + " leftleg_angle: " + toDegree(leftleg_angle) + " rightleg_angle: " + toDegree(rightleg_angle));

			// End Kerry

			var pose = new Pose(poseID, leftarm_angle, rightarm_angle);

			/* testing client.id vs expected_data */
			console.log("current client IS: " + client.id);
			console.log("expected data IS: " + expected_data);
			if (expected_data == "attacker") { // for getting attacker moves 
				attackerPoses.push(pose);

				/* send message to front end (sends to everyone connected for now) */
				for (de1 of wss.clients.values()) {
					if (de1.id == "host") {
						expected_data = "default";
						var msg = "p" + poseID + " " + leftarm_angle + " " + rightarm_angle;
						de1.send(msg);
					}
				}
			}
			else if (expected_data == "defender") { // for getting defender moves 
				var score = 100 - 16 * Math.round(abs(pose.leftArm - attackerPoses[pose.beat].leftArm));
				console.log("score: " + score);
				// var score = 100;
				/* send message to front end (sends to everyone connected for now) */
				for (de1 of wss.clients.values()) {
					if (de1.id == "host") {
						expected_data = "default";
						var msg = "s" + poseID + " " + score;
						// var msg = "s" + poseID + " " + "100";
						de1.send(msg);
					}
				}
			}
			else {
				expected_data = "default";
				console.log("ERROR: CLIENT ID WHEN RECEIVING POSE IS BAD");
			}
		}
	});

	//Method notifies when client disconnects
	client.on('close', () => {
		// connectionCount--;
		console.log('This Connection Closed!');
		console.log("Removing Client: " + client.id);
		if (client.id == "host") {
			hostConnected = 0;
		}
		else if (client.id == "attacker") {
			atkConnected = 0;
		}
		else if (client.id == "defender") {
			defConnected = 0;
		}
		else {
			console.log("mystery client disconnected");
		}
	});

});

wss.on('listening', () => {
	console.log('listening on 8080')
});

/**
 * Calculates the angle (in radians) between two vectors pointing outward from one center.
 * From stackoverflow: https://stackoverflow.com/a/7505937
 *
 * @param p0 first point
 * @param p1 second point
 * @param c center point
 */
function find_angle(p0,p1,c) {
    var p0c = Math.sqrt(Math.pow(c.x-p0.x,2)+
                        Math.pow(c.y-p0.y,2)); // p0->c (b)   
    var p1c = Math.sqrt(Math.pow(c.x-p1.x,2)+
                        Math.pow(c.y-p1.y,2)); // p1->c (a)
    var p0p1 = Math.sqrt(Math.pow(p1.x-p0.x,2)+
                         Math.pow(p1.y-p0.y,2)); // p0->p1 (c)
	if (p0.y > p1.y) { // desired angle is greater than 180 deg
		return 2 * Math.PI - Math.acos((p1c*p1c+p0c*p0c-p0p1*p0p1)/(2*p1c*p0c))
	}
	else { // desired angle is less than 180 deg
		return Math.acos((p1c*p1c+p0c*p0c-p0p1*p0p1)/(2*p1c*p0c));
	}
}

/**
 * Calculates the absolute value of a given number
 */
function abs(number) {
    if (number < 0) {
        return number * -1;
    }
    return number;
}