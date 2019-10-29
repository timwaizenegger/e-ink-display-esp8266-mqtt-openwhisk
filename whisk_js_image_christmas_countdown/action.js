/*
 * 
 * Tim Waizenegger (c) 2018,2019
 * Used as an open whisk action on IBM cloud

 * npm install
 * zip -r p.zip .
 * bx wsk action update displaychannel_image_christmas_countdown --kind nodejs:8 p.zip 

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
var fs = require('fs');
var path = require('path');

var d_now = new Date();
var d_christmas = new Date(d_now.getFullYear() +" December 24");
var millis_day = 60*60*24*1000;
var d_left = Math.floor((d_christmas - d_now) / millis_day);

function getLocalTimeString() {
  var Moment = require('moment-timezone');
  return Moment().tz('Europe/Berlin').format("HH:mm");
}

function myAction(args) {
    console.log("hi");
    time_string = getLocalTimeString();
    
    
    return new Promise(function(resolve, reject) {
    var image = new Jimp(400, 300, function (err, image) {
    image.rgba(false);
    console.log("generating image");
    
    
    Jimp.loadFont(Jimp.FONT_SANS_64_BLACK, function (err, font) {
    //var fontPath = path.join(__dirname, "font-hel.fnt");
    //Jimp.loadFont(fontPath, function (err, font) { 
      image.print(font, 10, 100, "Nur " + d_left + " Tage");  
      Jimp.loadFont(Jimp.FONT_SANS_32_BLACK, function (err, font) {
        image.print(font, 10, 10, "Ho! Ho! Ho!"); 
        image.print(font, 10, 210, "Dann ist Weihnachten :D"); 
        image.getBase64( Jimp.MIME_PNG, function (err, base64) {
            console.log(err);
            console.log(base64);
            resolve({ "message": "created text", "base64_data": base64 });
          });     
      });
    });
  })
    
    

    
    
  });
    
    
    
    
}

exports.main = myAction;
