/*
 * 
 * Tim Waizenegger (c) 2018,2019
 * Used as an open whisk action on IBM cloud

 * npm install
 * zip -r p.zip .
 * bx wsk action create make_clockimage --kind nodejs:8 p.zip 
 * bx wsk action update make_clockimage --kind nodejs:8 p.zip 

 * 
 * test locally:
 * nodejs -e 'require ("./action.js").main()'
 * 
 * make new bitmap fonts with this nice tool:
 * https://github.com/dmytrolev/bitmap-font-generator
 * 
 * 
 * */

var Jimp = require("jimp");
var datetime = require('node-datetime');
var fs = require('fs');
var path = require('path');

function myAction(args) {
    console.log("hi");
    d = datetime.create()
    d.offsetInHours(1);
    //console.log(d.format('m/d/y H:M'));
    console.log(d.format('H:M'));
    time_string = d.format('H:M');
    
    
    return new Promise(function(resolve, reject) {
    var image = new Jimp(400, 300, function (err, image) {
		image.rgba(false);
		console.log("generating image");
		
		
		//Jimp.loadFont(Jimp.FONT_SANS_128_BLACK, function (err, font) {
		var fontPath = path.join(__dirname, "font-hel.fnt");
		Jimp.loadFont(fontPath, function (err, font) {
			image.print(font, 20, 100, time_string); 
			image.getBase64( Jimp.MIME_PNG, function (err, base64) {
					console.log(err);
					console.log(base64);
					resolve({ "message": "created text", "base64_data": base64 });
				});			
		});
	})
		
		

    
    
	});
    
    
    
    
}

exports.main = myAction;
