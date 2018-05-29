var fs = require('fs-extra');
var  chalk = require('chalk');
var hb = require('handlebars');
var spawn = require('child_process').spawnSync;
var path = require('path');
var processor = require('../../utils/processor');
var error = chalk.bold.red;
var warning = chalk.yellow;

var Cmd = function() {

};

Cmd.prototype.run = function(app, dir) {
    console.log('Upgrading project in %s', dir);
    try {
        var files = fs.readdirSync(dir);
        if (files.length === 0) {
            console.log(error('Directory %s is empty'), dir);
            return;
        }
    } catch (e) {
        console.log(error('Directory %s does not exist'), dir);
        return;
    }

    var projectFile = require(path.resolve(dir)+'/binocle.json');
    var project = projectFile.project;

    if (project.engine != 'binocle' && project.engine != 'binocle-c') {
        console.log(error('Wrong engine specified: %s'), project.engine);
        return;
    }

    if (project.engine == "binocle") {
        if (!this.upgradeBinocle(dir)) {
            console.log(error("Error while upgrading Binocle in directory %s"), dir);
            return;
        }
    } else {
        if (!this.upgradeBinocleC(dir)) {
            console.log(error("Error while upgrading Binocle in directory %s"), dir);
            return;
        }
    }

    console.log("Project %s upgraded.", dir);
};

Cmd.prototype.upgradeBinocle = function(dir) {
    console.log("Upgrading binocle");
    try {
        var child = spawn('/usr/bin/git', ['-C', dir+'/Binocle', 'pull', 'origin']);
        if (child.error) {
            console.log(error(err.message));
            return false;
        }
    } catch (err) {
        console.log(error(err.message));
        return false;
    }
    return true;
};

Cmd.prototype.upgradeBinocleC = function(dir) {
    console.log("Upgrading binocle-c");
    try {
        var child = spawn('/usr/bin/git', ['-C', dir+'/binocle-c', 'pull', 'origin']);
        if (child.error) {
            console.log(error(err.message));
            return false;
        }
    } catch (err) {
        console.log(error(err.message));
        return false;
    }
    return true;
};

exports = module.exports = new Cmd();