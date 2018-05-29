var fs = require('fs-extra');
var chalk = require('chalk');
var hb = require('handlebars');
var spawn = require('child_process').spawnSync;
var path = require('path');
var processor = require('../../utils/processor');
var error = chalk.bold.red;
var warning = chalk.yellow;

var Cmd = function() {

};

Cmd.prototype.run = function(app, dir) {
    console.log('Updating project in %s', dir);
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

    console.log("Looking for project file %s", path.resolve(dir)+'/binocle.json');
    var projectFile = require(path.resolve(dir)+'/binocle.json');
    var project = projectFile.project;
    if (project.engine != 'binocle' && project.engine != 'binocle-c') {
        console.log(error('Wrong engine specified: %s'), project.engine);
        return;
    }

    processor.processFile(path.join(__dirname, '../../templates/' + project.engine + '/CMakeLists.txt'), dir+'/CMakeLists.txt', {project_name:project.name});

    console.log("Project %s updated.", dir);
    
};

exports = module.exports = new Cmd();