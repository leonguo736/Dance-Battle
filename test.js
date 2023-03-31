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
    return Math.acos((p1c*p1c+p0c*p0c-p0p1*p0p1)/(2*p1c*p0c));
}
var point1 = new Object();
point1.x = 1;
point1.y = 1;
var center = new Object();
center.x = 0;
center.y = 0;
var point2 = new Object();
point2.x = 1;
point2.y = 0;
console.log("angle: " + find_angle(point1, point2, center));

var beatArray_atk = [];
var leftArray_atk = [];
var rightArray_atk = [];
var beatArray_def = [];
var leftArray_def = [];
var rightArray_def = [];
var score = 0;

var testString1 = "p20,26,21,";
var testString2 = "p5,7,5,";

// add if statement later to only read from attacker. also rename arrays to differentiate between atk and def. 
// then compare atk vs def to get score

var string1 = testString1.substring(1, testString1.length - 1);
var string2 = testString2.substring(1, testString2.length - 1);
console.log("string1: " + string1 + " string2: " + string2);

var dataArray_atk = string1.split(",");
var dataArray_def = string2.split(",");
var numPoses = (dataArray_atk.length)/3;
for (var i = 0; i < numPoses; i++) {
    beatArray_atk.push(dataArray_atk[i]);
    leftArray_atk.push(dataArray_atk[i+1]);
    rightArray_atk.push(dataArray_atk[i+2]);
}
for (var i = 0; i < numPoses; i++) {
    beatArray_def.push(dataArray_def[i]);
    leftArray_def.push(dataArray_def[i+1]);
    rightArray_def.push(dataArray_def[i+2]);
}

for (var i = 0; i < numPoses; i++) {
	beatArray_atk.push(dataArray_atk[i]);
	leftArray_atk.push(dataArray_atk[i+1]);
	rightArray_atk.push(dataArray_atk[i+2]);
	console.log("pushed: " + dataArray_atk[i+2]);
}
for (var i = 0; i < numPoses; i++) {
	beatArray_def.push(dataArray_def[i]);
	leftArray_def.push(dataArray_def[i+1]);
	rightArray_def.push(dataArray_def[i+2]);
	console.log("pushed: " + dataArray_def[i+2]);
}

for (var i = 0; i < numPoses; i++) {
    score += 25 - abs(Number.parseInt(leftArray_atk[i]) - Number.parseInt(leftArray_def[i]));
    console.log("score: " + score);
    score += 25 - abs(Number.parseInt(rightArray_atk[i]) - Number.parseInt(leftArray_def[i]));
    console.log("score: " + score);
}

client.on('message', (data) => {
    parsedData = JSON.parse(data); // coordinate array looks like this: [[0,0], [1,1], [2,2]]

    /* determine what to do based on command field */
    if (parsedData.command == "camera") {
        /* set up the 5 points. chest left right */ 
        var leftArm = new Point(parsedData.median[0], parsedData.median[1]);
        var leftElbow = new Point(parsedData.median[2], parsedData.median[3]);
        var rightArm = new Point(parsedData.median[4], parsedData.median[5]);
        var rightElbow = new Point(parsedData.median[6], parsedData.median[7]);
        var chest = new Point(parsedData.median[8], parsedData.median[9]);

        /* calculate the 2 angles and store data in Pose objects */
        var leftarm_angle = find_angle(leftArm, chest, leftElbow);
        var rightarm_angle = find_angle(rightArm, chest, rightElbow);
        var beat = 1; // temporary placeholder
        var pose = new Pose(beat, leftarm_angle, rightarm_angle);
        
        /* send message to front end (sends to everyone connected for now) */
        for (client of wss.clients.values()) {
            client.send(JSON.stringify(pose));
        }
    }
});