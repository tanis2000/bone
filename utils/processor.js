var fs = require('fs-extra');
var  chalk = require('chalk');
var hb = require('handlebars');
var spawn = require('child_process').spawnSync;
var error = chalk.bold.red;
var warning = chalk.yellow;

var Processor = function() {

};

Processor.prototype.processFile = function(src, dst, context) {
    var buf = fs.readFileSync(src, 'utf8');
    var template = hb.compile(buf.toString());
    var res = template(context);
    fs.writeFileSync(dst, res);
};

exports = module.exports = new Processor();