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
    console.log('Building project in %s for target platform %s', dir, target);
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

    if (!this.buildProject(dir, target)) {
        return;
    }

    console.log("Project %s built.", dir);
};

Cmd.prototype.buildProject = function(dir, target) {
    var projectFile = this.loadProject(dir);
    if (typeof projectFile === "undefined" || projectFile == '') {
        console.log(error('Cannot read the binocle.json project file'));
        return false;
    }
    var ios_development_team_id = projectFile.project.ios_development_team_id;
    var parameters = [];
    var command = 'cmake';

    if (target === "mac") {
        parameters = ['-G', 'Xcode', '-D', 'DEBUG=1', '../..'];
    } else if (target === "windows") {
        parameters = ['-G', 'Unix Makefiles', '-D', 'DEBUG=1', '../..'];
    } else if (target === "linux") {
        parameters = ['-G', 'Unix Makefiles', '-D', 'DEBUG=1', '../..'];
    } else if (target === "ios") {
        parameters = ['-G', 'Xcode', '-D', 'DEBUG=1', '-D', 'IOS=1', '-D', 'IOS_DEVELOPMENT_TEAM_ID='+ios_development_team_id, '../..'];
    } else if (target === "android") {
        parameters = ['-G', 'Unix Makefiles', '-D', 'DEBUG=1', '-D', 'ANDROID_ABI=armeabi-v7a', '-D', 'ANDROID_STL=gnustl_static', '-D', 'ANDROID_NATIVE_API_LEVEL=android-17', '-D', 'CMAKE_TOOLCHAIN_FILE=../Binocle/build/cmake/android.toolchain.cmake', '../..'];
    } else if (target === "web") {
        command = 'emcmake';
        parameters = ['cmake', '../..', '-DCMAKE_BUILD_TYPE=Release'];
    } else {
        console.log(error("Do not know how to configure for this target."));
        return false;
    }

    fs.mkdirsSync(dir+'/build/'+target);

    try {
        var child = exec(command, 
            parameters,
            { cwd: dir+'/build/'+target}
        );
    } catch (err) {
        console.log(error(err.message));
        return false;
    }

    if (target === "mac") {
        command = 'xcodebuild';
        parameters = [];
    } else if (target === "windows") {
        console.log(error("Do not know how to build for this target. Please submit a PR if you can help with Windows and Visual Studio scripting"));
        return false;
    } else if (target === "linux") {
        command = 'make';
        parameters = [];
    } else if (target === "ios") {
        command = 'xcodebuild';
        parameters = ['-allowProvisioningUpdates', '-sdk', 'iphonesimulator'];
    } else if (target === "android") {
        command = 'make';
        parameters = [];
        try {
            var child = exec(command, 
                parameters,
                { 
                    cwd: dir+'/build/'+target,
                    maxBuffer: 1024*1024*1024,
                }
            );
        } catch (err) {
            console.log(error(err.message));
            return false;
        }

        command = 'gradlew';
        parameters = ['assembleDebug'];
        try {
            var child = exec(command, 
                parameters,
                { 
                    cwd: dir+'/build/'+target+'/android-project',
                    maxBuffer: 1024*1024*1024,
                }
            );
        } catch (err) {
            console.log(error(err.message));
            return false;
        }
    } else if (target === "web") {
        command = 'make';
        parameters = ['-j8'];
    } else {
        console.log(error("Do not know how to build for this target."));
        return false;
    }

    try {
        var child = exec(command, 
            parameters,
            { 
                cwd: dir+'/build/'+target,
                maxBuffer: 1024*1024*1024,
            }
        );
    } catch (err) {
        console.log(error(err.message));
        return false;
    }

    return true;
};

Cmd.prototype.loadProject = function(dir) {
    var projectFile = require(path.resolve(dir)+'/binocle.json');
    return projectFile;
};


exports = module.exports = new Cmd();