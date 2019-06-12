var fs = require('fs-extra');
var  chalk = require('chalk');
var hb = require('handlebars');
var spawn = require('child_process').spawnSync;
var exec = require('child_process').execFileSync;
var path = require('path');
var processor = require('../../utils/processor');
var config = require('../../config.json');

var error = chalk.bold.red;
var warning = chalk.yellow;

var Cmd = function() {

};

Cmd.prototype.run = function(app, dir, target) {
    console.log('Running project in %s for target platform %s', dir, target);
    if (config.build.supported_targets.indexOf(target) < 0) {
        console.log(error("Target %s not supported"), target);
        return;
    }

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

    if (!this.runProject(dir, target)) {
        return;
    }

    console.log("Project %s run.", dir);
};

Cmd.prototype.loadProject = function(dir) {
    var projectFile = require(path.resolve(dir)+'/binocle.json');
    return projectFile;
};

Cmd.prototype.runCommand = function(command, parameters, cwd) {
    try {
        console.log("Running: " + command + " with parameters " + parameters + " in directory " + cwd);
        var child = exec(command, 
            parameters,
            { 
                cwd: cwd,
                maxBuffer: 1024 * 1024 * 1024
            }
        );
    } catch (err) {
        console.log(error(err.message));
        return false;
    }
};

Cmd.prototype.runCommandWithOutput = function(command, parameters, cwd) {
    try {
        console.log("Running: " + command + " with parameters " + parameters + " in directory " + cwd);
        var child = exec(command, 
            parameters,
            { 
                cwd: cwd,
                maxBuffer: 1024 * 1024 * 1024
            }
        );
        return child.toString();
    } catch (err) {
        console.log(error(err.message));
        return '';
    }
};

Cmd.prototype.runProject = function(dir, target) {
    var projectFile = this.loadProject(dir);
    if (typeof projectFile === "undefined" || projectFile == '') {
        console.log(error('Cannot read the binocle.json project file'));
        return false;
    }

    if (target === "mac") {
        var command = 'open';
        var parameters = [dir + '/build/' + target + '/src/' + projectFile.project.name + '.app'];
        var cwd = dir;
        return this.runCommand(command, parameters, cwd);
    } else if (target === "windows") {
        console.log(error("Do not know how to run for this target. Please submit a PR if you know what to do"));
        return false;
    } else if (target === "linux") {
        console.log(error("Do not know how to run for this target. Please submit a PR if you know what to do"));
        return false;
    } else if (target === "ios") {
        var applicationPath = dir + '/build/' + target + '/src/' + projectFile.project.name + '.app';
        var iosBundleId = projectFile.project.ios_bundle_id;
        var command = 'xcrun';
        var parameters = ['simctl', 'list', '-j', 'devices'];
        var cwd = dir;
        var output = this.runCommandWithOutput(command, parameters, cwd);
        var obj = JSON.parse(output);
        console.log(obj);
        var selectedDevice = null;
        for (var device in obj.devices) {
            if (device.includes('iOS')) {
                selectedDevice = obj.devices[device];
            }
            /*
            if (obj.devices.hasOwnProperty(device)) {
                console.log(device);
                console.log(obj.devices[device]);
            }
            */
        }
        console.log(selectedDevice);
        var selectedSimulator = selectedDevice[0];
        var selectedDeviceID = selectedSimulator.udid;
        this.runCommand('open', ['-a', 'Simulator', '--args', '-CurrentDeviceUDID', selectedDeviceID]);
        this.runCommand('xcrun', ['simctl', 'uninstall', selectedDeviceID, iosBundleId]);
        this.runCommand('xcrun', ['simctl', 'install', selectedDeviceID, applicationPath]);
        this.runCommand('xcrun', ['simctl', 'launch', selectedDeviceID, iosBundleId]);
        return true;
    } else if (target === "android") {
        console.log(error("Do not know how to run for this target. Please submit a PR if you know what to do"));
        return false;
    } else if (target === "web") {
        console.log(error("Do not know how to run for this target. Please submit a PR if you know what to do"));
        return false;
    } else {
        console.log(error("Do not know how to configure for this target."));
        return false;
    }

    return true;
};


exports = module.exports = new Cmd();