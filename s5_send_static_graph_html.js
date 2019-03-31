var express = require('express')
var app = express()
fs = require('fs');
var content = fs.readFileSync('./usr.txt','utf-8');
var usr = content.toString().split("\n");

mysql = require('mysql');
var connection = mysql.createConnection({
    host: usr[0],
    user: usr[1],
    password: usr[2],
    database: usr[3]
    /*
    host: 'localhost',
    user: 'me',
    password: 'mypassword',
    database: 'mydb'
    */
})
connection.connect();


app.get('/graph', function (req, res) {
    console.log('got app.get(graph)');
    var html = fs.readFile('./graph6.html', function (err, html) {
    html = " "+ html
    console.log('read file');

    var qstr = 'select * from sensors ';

    var begin;
    var end;

    connection.query(qstr, function(err, rows, cols) {
      if (err) throw err;

      var data = "";
      var comma = "";
      var r_t;
      var t;
      var s;
      start = rows[0].time.toLocaleString();
      for (var i=0; i< rows.length; i++) {
         r = rows[i];
         r_t = r.time;
         t = r_t.toLocaleString();
         t = t.replace(/-/gi,',');
         t = t.replace(/:/gi,',');
 
