/*

 * Tim Waizenegger (c) 2018,2019
 * Used as an open whisk action on IBM cloud

 * npm install
 * zip -r p.zip .
 * bx wsk action update displaychannel_image_twitter --kind nodejs:8 p.zip 

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

var bearer = "twitter api bearer token"
var screen_name = "CatFacts101"


function myAction(args) {
    console.log("hi");

    var headers = {"Authorization": "Bearer " + bearer}


  return new Promise(function(resolve, reject) {
    request.get({url: "https://api.twitter.com/1.1/statuses/user_timeline.json?tweet_mode=extended&trim_user=1&count=100&screen_name=" + screen_name,
                 headers: headers}, function (error, response, body) {
      console.log(body);
      var timeline = JSON.parse(body)
      //var this_tweet = timeline[0]
      var this_tweet = timeline[Math.floor(Math.random()*timeline.length)];
      tweet_txt = this_tweet.full_text
      console.log(tweet_txt)
      
      var tweet_lines = [];
      var line_length = 25;

      for (var i = 0; i < tweet_txt.length; i += line_length) {
          tweet_lines.push(tweet_txt.substring(i, i + line_length));
      }
      
      console.log(tweet_lines);
      
     
        var image = new Jimp(400, 300, function (err, image) {
        image.rgba(false);
        console.log("generating image");
        
        
        Jimp.loadFont(Jimp.FONT_SANS_64_BLACK, function (err, font) {
          image.print(font, 5, 1, "@" + screen_name);  
          Jimp.loadFont(Jimp.FONT_SANS_32_BLACK, function (err, font) { 
            for (var j = 0; j < tweet_lines.length; j++) {
                console.log(tweet_lines[j]);
                image.print(font, 10, 80+30*j, tweet_lines[j]); 
              }
            
            
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
