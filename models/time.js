var mongoose = require('mongoose');
var Schema = mongoose.Schema;

var schema = new Schema({
  CurrentDateAndTime: {type: String, required: true},



});
module.exports = mongoose.model('Time',schema);
