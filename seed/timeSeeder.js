var Time = require('../models/time');
var mongoose = require('mongoose');



var times = [
   new Time({
CurrentDateAndTime: 'timeseeder is working'

}),

new Time({
CurrentDateAndTime: '2021-04-17T08:02+01:00'

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
  mongoose.disconnect();
}
