const WebSocket = require('ws');

const wss = new WebSocket.WebSocketServer({ port: 8080 }, () => {
	console.log('INFO: server started');
});

/* Variables used to keep track of DE1-SoC states */
var atkConnected = 0;
var defConnected = 0;
var hostConnected = 0;
var expected_data = "default";

/**
 * Object used to store pose information
 * 
 * beat: corresponds to the beat of a song at which the pose will be displayed. Also used as a unique identifier.
 * leftArm: angle from x-axis to the player's leftArm. Measured in radians.
 * rightArm: angle from x-axis to the player's rightArm. Measured in radians.
 * leftLeg: angle from x-axis to the player's leftLeg. Measured in radians.
 * rightLeg: angle from x-axis to the player's rightLeg. Measured in radians.
 */
class Pose {
	constructor(beat, leftArm, rightArm, leftLeg, rightLeg) {
		this.beat = beat;
		this.leftArm = leftArm;
		this.rightArm = rightArm;
		this.leftLeg = leftLeg;
		this.rightLeg = rightLeg;
	}
}

/* Point objects that are used to store coordinates sent by cameras. Cameras consider the top-left corner to be [0,0], so adjust y-value */
class Point {
	constructor(x, y) {
		this.x = x;
		this.y = 480 - y;
	}
}

var attackerPoses = []; 
var gamemode = 0; // set to zero to play with 1 attacker and 1 defender. set to 1 to play with 2 defenders
var song;
var ymca = [
	[0, 1, 8, 135, 45],
	[1, 1, 10, 250, 290],
	[2, 1, 11, 60, -30],
	[3, 1, 11.5, 225, -45],
	[4, 1, 16, 135, 45],
	[5, 1, 18, 250, 290],
	[6, 1, 19, 60, -30],
	[7, 1, 19.5, 225, -45],
	[8, 1, 24, 225, 250],
	[9, 1, 25, 290, 315],
	[10, 1, 26.5, 160, 135],
	[11, 1, 28, 290, 315],
	[12, 1, 29, 225, 250],
	[13, 1, 30.5, 45, 20],
	[14, 1, 32, 250, 290],
	[15, 1, 33, 215, 325],
	[16, 1, 34.5, 180, 0],
	[17, 1, 35.25, 145, 35],
	[18, 1, 36.5, 110, 70],
	[19, 1, 40, 135, 45],
	[20, 1, 42, 250, 290],
	[21, 1, 43, 60, -30],
	[22, 1, 43.5, 225, -45],
	[23, 1, 48, 135, 45],
	[24, 1, 50, 250, 290],
	[25, 1, 51, 60, -30],
	[26, 1, 51.5, 225, -45]];

/* Websocket function that managages connection with clients */
wss.on('connection', function connection(client) {
	console.log("INFO: websocket on connection");

	/**
	 * Server expects each message that it receives to be in JSON format and must contain a "command" field. 
	 * Throws an error message otherwise.
	 */
	client.on('message', (data) => {
		try {
			var parsedData = JSON.parse(data);
		} catch (e) {
			console.log(`ERROR: Server received incorrectly formatted JSON string: ${data}`);
			
			var parsedData = new Object();
			parsedData.command = "bad json received";
		}

		if (parsedData.command == "lobbyInit") { // to send information regarding connected DE1s to the host
			console.log("INFO: recieved command lobbyInit");
			client.send("l" + atkConnected + " " + defConnected);
		}
		else if (parsedData.command == "setMode") { // to toggle between gamemodes
			console.log(`INFO: received command setMode: ${data}`);
			if ((gamemode == 0) && (parsedData.type == 1)) {
				for (de1 of wss.clients.values()) {
					if (de1.id == "attacker") {
						console.log("switched attacker to defender0");
						de1.send("id,2");
					}
				}
			}

			gamemode = parsedData.type;
			song = parsedData.song;
		}
		else if (parsedData.command == "setType") { // depending on the value of setType, set the identity of the connected DE1
			if (parsedData.identifier == "host") {
				hostConnected = 1;
				client.id = "host";
				console.log("INFO: received command setType host");

				client.send("l" + atkConnected + " " + defConnected);
			}
			else if (parsedData.identifier == "camera") { // server determines whether a camera connection takes the role of attacker or defender
				if (atkConnected == 0) {
					console.log("INFO: received command setType attacker id,1. Gamemode is " + gamemode);
					atkConnected = 1;
					client.id = "attacker";
					
					var data = gamemode == 0 ? "id,1" : "id,2";
					client.send(data);

					for (de1 of wss.clients.values()) {
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
					console.log("INFO: received command setType defender id,2");
					client.send(data);

					for (de1 of wss.clients.values()) {
						if (de1.id == "host") {
							console.log("sending data to host");
							de1.send("l" + atkConnected + " " + defConnected);
						}
					}
				}
				else {
					console.log("ERROR: extra client connected");
				}
			}
		}
		else if (parsedData.command == "captureAttacker") { // host sent a request for an attacker pose. Give the signal to capture an image.
			process.stdout.write("INFO: sending signal to attacker ... ");
			for (de1 of wss.clients.values()) {
				if (de1.id == "attacker") {
					process.stdout.write("sent poseId " + parsedData.poseID + " to attacker");

					expected_data = "attacker";
					de1.send("cap," + parsedData.poseID);
				}
			}
		}
		else if (parsedData.command == "captureDefender") { // host sent a request for a defender pose. Give the signal to capture an image.
			process.stdout.write("INFO: sending signal to defender ... ");
			if (gamemode == 0) {
				for (de1 of wss.clients.values()) {
					if (de1.id == "defender") {
						process.stdout.write("sent poseID " + parsedData.poseID + " to defender"); 
	
						expected_data = "defender";
						de1.send("cap," + parsedData.poseID);
					}
				}
			}
			else if (gamemode == 1) {
				for (de1 of wss.clients.values()) {
					if (de1.id == "attacker" || de1.id == "defender") {
						process.stdout.write("sent poseID " + parsedData.poseID + " to defender"); 
	
						expected_data = "defender";
						de1.send("cap," + parsedData.poseID);
					}
				}
			}
			else {
				console.log("ERROR: unknown gamemode when sending capture cmd to defender(s)");
			}
		}
		else if (parsedData.command == "poseData") { // for handling data sent from cameras
			console.log("INFO: recieved poseData: " + JSON.stringify(parsedData.median));
			/**
			 * Assumes JSON has 3 fields: "command", "median" and "poseID". Also assumes only one pose is sent per message.
			 * 
			 * Function takes numbers from "median" array and uses them to create 6 Point objects. 
			 * The points are passed through getAngle() which computes and angle in radians.
			 * A pose object is then created using 6 angles and a pose ID, which is then sent to the host.
			 */

			/* set up points */
			var chest = new Point(parsedData.median[0][0], parsedData.median[0][1]);
			var leftHand = new Point(parsedData.median[1][0], parsedData.median[1][1]);
			var rightHand = new Point(parsedData.median[2][0], parsedData.median[2][1]);
			var pelvis = new Point(parsedData.median[3][0], parsedData.median[3][1]);
			var leftLeg = new Point(parsedData.median[4][0], parsedData.median[4][1]);
			var rightLeg = new Point(parsedData.median[5][0], parsedData.median[5][1]);
			
			leftarm_angle = getAngle(chest, leftHand); // give to alex, counter-cw for alex
			rightarm_angle = getAngle(chest, rightHand); // give to alex, counter-cw for alex
			leftleg_angle = getAngle(pelvis, leftLeg); // give to alex, counter-cw for alex
			rightleg_angle = getAngle(pelvis, rightLeg); // give to alex, counter-cw for alex

			console.log("leftarm_angle: " + toDegree(leftarm_angle) + " rightarm_angle: " + toDegree(rightarm_angle) + " leftleg_angle: " + toDegree(leftleg_angle) + " rightleg_angle: " + toDegree(rightleg_angle));

			var poseID = parsedData.poseId;
			var pose = new Pose(poseID, leftarm_angle, rightarm_angle, leftleg_angle, rightleg_angle);
			console.log("-------");
			console.log(pose);
			console.log("-------");

			if (expected_data == "attacker") { // for getting attacker moves 
				attackerPoses.push(pose);

				/* send message to front end */
				for (de1 of wss.clients.values()) {
					if (de1.id == "host") {
						expected_data = "default";
						var msg = "p" + poseID + " " + -leftarm_angle + " " + -rightarm_angle + " " + -leftleg_angle + " " + -rightleg_angle;
						de1.send(msg);
					}
				}
			}
			else if ((expected_data == "defender") || (expected_data == "default")) { // for getting defender moves 
				if (gamemode == 1) {
					var score_rightArm = getScore(pose.rightArm, toRad(ymca[pose.beat][4]));
					var score_leftArm = getScore(pose.leftArm, toRad(ymca[pose.beat][3]));
					var score_rightLeg = getScore(pose.rightArm, toRad(ymca[pose.beat][4]));
					var score_leftLeg = getScore(pose.leftArm, toRad(ymca[pose.beat][3]));

					var score = score_rightArm + score_leftArm + score_rightLeg + score_leftLeg;

					/* tell host which defender is sending data */
					if (client.id == "attacker") {
						de1ID = "o";
					}
					else if (client.id == "defender") {
						de1ID = "t";
					}
					else {
						console.log("ERROR: expected client id of either attacker or defender. Got " + client.id + "instead");
					}

					/* send message to front end */
					for (de1 of wss.clients.values()) {
						if (de1.id == "host") {
							expected_data = "default";
							console.log("sending score (mode 1): " + score);
							var msg = de1ID + poseID + " " + score;
							de1.send(msg);
						}
					}
				}
				else {
					var attackerPose = "dummy";

					for (pose_atk of attackerPoses) {
						if (pose.beat == pose_atk.beat) {
							attackerPose = pose_atk;
						}
					}
					if (attackerPose == "dummy") {
						console.log("ERROR: could not find attacker pose with corresponding ID: " + pose.beat);
					}
					else {
						var score_rightArm = getScore(pose.rightArm, attackerPose.rightArm);
						var score_leftArm = getScore(pose.leftArm, attackerPose.leftArm);
						var score_rightLeg = getScore(pose.rightLeg, attackerPose.rightLeg);
						var score_leftLeg = getScore(pose.leftLeg, attackerPose.leftLeg);
						var score = score_rightArm + score_leftArm + score_rightLeg + score_leftLeg;
					}
					/* send message to front end */
					for (de1 of wss.clients.values()) {
						if (de1.id == "host") {
							expected_data = "default";
							console.log("sending score (mode 0): " + score);
							var msg = "s" + poseID + " " + score;
							de1.send(msg);
						}
					}
				}
			}
			else {
				console.log("ERROR: expected data is " + expected_data + ". Setting to default now...");
				expected_data = "default";
			}
		}
		else {
			console.log("ERROR: no command field in JSON string");
		}
	});

	// Method notifies when client disconnects
	client.on('close', () => {
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
			console.log("ERROR: expected client id of either host, attacker, or defender. Got " + client.id + "instead");
		}
		for (de1 of wss.clients.values()) {
			if (de1.id == "host") {
				console.log("sending data to host");
				de1.send("l" + atkConnected + " " + defConnected);
			}
		}
	});

});

wss.on('listening', () => {
	console.log('INFO: listening on 8080')
});


/**
 * Calculates the absolute value of a given number
 */
function abs(number) {
	if (number < 0) {
		return number * -1;
	}
	return number;
}

/** 
 * Calculates a score when given two pose objects
 * 
 */
function getScore(angle_atk, angle_def) {
	var score_raw = 25 - 4 * Math.round(abs(angle_atk - angle_def));
	var score = score_raw;
	if (score_raw > 22) {
		score = 25;
	}
	return score;
}

function toRad(d) {
	return d * (Math.PI / 180);
}
function toDegree(r) {
	return r * (180 / Math.PI);
}

function getAngle(a, b) {
	const deltaX = b.x - a.x;
	const deltaY = b.y - a.y;
	return Math.atan2(deltaY, deltaX);
}

function createPoint(x, y) {
	return new Point(x, 480 - y);
}