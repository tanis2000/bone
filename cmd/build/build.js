var fs = require('fs-extra');
var  chalk = require('chalk');
var hb = require('handlebars');
var spawn = require('child_process').spawnSync;
var exec = require('child_process').execFileSync;
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
    var parameters = [];
    var command = 'cmake';

    if (target === "mac") {
        parameters = ['-G', 'Xcode', '-D', 'DEBUG=1', '../..'];
    } else if (target === "windows") {
        parameters = ['-G', 'Unix Makefiles', '-D', 'DEBUG=1', '../..'];
    } else if (target === "linux") {
        parameters = ['-G', 'Unix Makefiles', '-D', 'DEBUG=1', '../..'];
    } else if (target === "ios") {
        parameters = ['-G', 'Unix Makefiles', '-D', 'DEBUG=1', '-D', 'IOS=1', '../..'];
    } else if (target === "android") {
        parameters = ['-G', 'Unix Makefiles', '-D', 'DEBUG=1', '-D', 'ANDROID_ABI=armeabi-v7a', '-D', 'ANDROID_STL=gnustl_static', '-D', 'ANDROID_NATIVE_API_LEVEL=android-17', '-D', 'CMAKE_TOOLCHAIN_FILE=../Binocle/build/cmake/android.toolchain.cmake', '../..'];
    } else if (target === "web") {
        command = 'emcmake';
        parameters = ['cmake', '../..', '-DCMAKE_BUILD_TYPE=Release'];
    } else {
        console.log(error("Do not know how to build for this target."));
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
    return true;
};


exports = module.exports = new Cmd();