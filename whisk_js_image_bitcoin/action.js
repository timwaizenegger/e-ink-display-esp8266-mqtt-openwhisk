/*
 * 
 * Tim Waizenegger (c) 2018,2019
 * Used as an open whisk action on IBM cloud

 * npm install
 * zip -r p.zip .
 * bx wsk action update displaychannel_image_bitcoin --kind nodejs:8 p.zip 

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

var request = require("request");



function myAction(args) {
    console.log("hi");



  return new Promise(function(resolve, reject) {
    request("https://api.coindesk.com/v1/bpi/currentprice.json", function (error, response, body) {
      console.log(body);
      var resp = JSON.parse(body)
      console.log(resp.bpi)
      var cb_eur = resp.bpi.EUR.rate_float
      console.log(cb_eur)
      //cb_eur = cb_eur.toLocaleString("eu-DE", {style: 'currency', currency: 'EUR'})
      cb_eur = cb_eur.toFixed()
      console.log(cb_eur)
      
        var image = new Jimp(400, 300, function (err, image) {
        image.rgba(false);
        console.log("generating image");
        
        
        Jimp.loadFont(Jimp.FONT_SANS_64_BLACK, function (err, font) {
          image.print(font, 10, 130, "Euro");  
          image.print(font, 10, 190, " == 1 Bitcoin");
          Jimp.loadFont(Jimp.FONT_SANS_128_BLACK, function (err, font) { 
            image.print(font, 10, 10, cb_eur); 
            image.getBase64( Jimp.MIME_PNG, function (err, base64) {
                console.log(err);
                console.log(base64);
                resolve({ "message": "created text", "base64_data": base64 });
              });     
          });
        });
      })
    });
      
  });
    
    
    
    
}

exports.main = myAction;
