var createError = require('http-errors');
var express = require('express');
var path = require('path');
var cookieParser = require('cookie-parser');
var logger = require('morgan');
var expressHbs = require('express-handlebars')
var mongoose = require('mongoose');
//var session = require('express-session');
var indexRouter = require('./routes/index');
var Time = require('./models/time');


var app = express();
// view engine setup
app.engine('.hbs',expressHbs({defaultLayout: 'layout', extname: '.hbs'}));
app.set('view engine', 'hbs');

app.use(logger('dev'));
app.use(express.json());
app.use(express.urlencoded({ extended: false }));
app.use(cookieParser());
app.use(express.static(path.join(__dirname, 'public')));
app.listen(process.env.PORT || 3000, process.env.IP || '0.0.0.0' );//new
app.use(express.static('views/images'));
app.use('/', indexRouter);

app.post('/post-feedback', function (req, res) {
  var times = [
     new Time({
  CurrentDateAndTime: JSON.stringify(req.body)

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
  //  res.send('Data received:\n' + JSON.stringify(req.body));
});


// catch 404 and forward to error handler
app.use(function(req, res, next) {
  next(createError(404));
});

// error handler
app.use(function(err, req, res, next) {
  // set locals, only providing error in development
  res.locals.message = err.message;
  res.locals.error = req.app.get('env') === 'development' ? err : {};

  // render the error page
  res.status(err.status || 500);
  res.render('error');
});

module.exports = app;
