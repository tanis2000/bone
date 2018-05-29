#!/usr/bin/env node

var program = require('commander');
var os = require('os');
var cmd = require('./cmd');

var app = {};

switch(os.platform()) {
    case "darwin":
    app.platform = "mac";
    break;
    case "linux":
    app.platform = "linux";
    break;
    case "win32":
    app.platform = "windows";
    break;
    default:
    app.platform = "unknown";
    break;
}

program
.version('0.1.0')
.command('init <project_name> <dir>')
.option('-e, --engine [engine]', 'specify the engine: [binocle, binocle-c]')
.description('initialize a new Binocle project')
.action(function(project_name, dir, options) {
    cmd.init.run(app, project_name, dir, options.engine);
});

program.command('update <dir>')
.description('update a Binocle project. Only the CMake files will be overwritten. Use with caution and make a backup of your project.')
.action(function(dir) {
    cmd.update.run(app, dir);
});

program.command('upgrade <dir>')
.description('upgrade the version of Binocle in an existing project.')
.action(function(dir) {
    cmd.upgrade.run(app, dir);
});

program.command('build <dir> [target]')
.description('build the project in <dir> for the target [target].')
.action(function(dir, target) {
    target = target || app.platform;
    cmd.build.run(app, dir, target);
});

program.parse(process.argv);