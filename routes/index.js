var express = require('express');
var router = express.Router();
var Time = require('../models/time');
var mongoose = require('mongoose');
var fetch = require("node-fetch");
var awsIot = require('aws-iot-device-sdk');
const time_url = 'http://worldclockapi.com/api/json/gmt/now';

/* GET home page. */
mongoose.connect("mongodb://localhost:27017/timestamps", { useNewUrlParser: true });

async function getTime(){
}

async function getAlarm(alarm){
  device.on('connect', function() {
      console.log('connect');
      device.subscribe(alarm);
    });
}

router.get('/', function(req, res, next) {
Time.find(function(err,docs){
    var timeChunks = [];
    chunkSize = 3;
    for(var i = 0; i < docs.length; i+= chunkSize ){
      timeChunks.push(docs.slice(i,i+chunkSize));
    }
    res.render('./home/index', { title: 'Cloud Security System', times: timeChunks });
  }).lean();
});

router.get('/info', function(req,res,next) {
  res.render('./home/info', { title: 'Information'});
})

router.get('/index', function(req,res,next) {
  Time.find(function(err,docs){
      var timeChunks = [];
      chunkSize = 3;
      for(var i = 0; i < docs.length; i+= chunkSize ){
        timeChunks.push(docs.slice(i,i+chunkSize));
      }
      res.render('home/index', { title: 'Cloud Security System', times: timeChunks });
    }).lean();
})

//auth key and cert
var device = awsIot.device({
  keyPath: './certs/private.pem.key',
  certPath: './certs/certificate.pem.crt',
  caPath: './certs/AmazonRootCA1.pem',
  clientId: "qwerty",
  host: 'xyz.iot.us-east-1.amazonaws.com'
});

getAlarm('TOUCH');
getAlarm('HALL');

device.on('message', function(topic, payload) {
  console.log('message', topic, payload.toString());

  getTime();
  var times = [
    new Time({
      CurrentDateAndTime: JSON.stringify(payload.toString())
    })
  ];

  var done =0;
  for(var i = 0; i< times.length; i++)
  {
    times[i].save(function(err,result) {
    done++;
      if(done === times.length)
      {
        exit();
      }
    });
  }

  function exit()
  {
    //mongoose.disconnect();
  }
});


module.exports = router;
