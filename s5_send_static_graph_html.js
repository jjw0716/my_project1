var express = require('express')
var app = express()
//require 사용!!
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
         t = t.replace(/ /gi,',');
         s = t.split(',');
         data += comma + "[new Date("+s[0]+","+s[1]+'-1'+","+s[2]+","+s[3]+","+s[4]+","+s[5]+")," + r.value +"]";
         comma = ",";
      }
      end = r_t.toLocaleString();
      var header = "data.addColumn('date', 'Date/Time');"
      header += "data.addColumn('number', 'Temp');"
      html = html.replace("<%HEADER%>", header);
      html = html.replace("<%DATA%>", data);
      var title = "title: 'Temperature Sensor Data!! by Jeong Ji Wook\\n";
            title += "started at ";
            title+= start;
            title+= "\\n ended at ";
            title+= end;
            title+= "',";
      html = html.replace("<%TITLE%>",title);
      var subtitle = "subtitle: 'node.js gitbub url: ";
           subtitle += usr[4];
            subtitle += "\\n";
            subtitle += "arduino github url: "
            subtitle += usr[5];
            subtitle += "'";
      html = html.replace("<%SUBTITLE%>",subtitle);

      res.writeHeader(200, {"Content-Type": "text/html"});
      res.write(html);
      res.end();
    });
  });
})

var server = app.listen(8000, function () {
  var host = server.address().address
  var port = server.address().port
  console.log('listening at http://%s:%s', host, port)
});
