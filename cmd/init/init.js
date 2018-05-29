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

Cmd.prototype.run = function(app, project_name, dir, engine) {
    if (engine != 'binocle' && engine != 'binocle-c') {
        console.log(error('Wrong engine specified: %s'), engine);
        return;
    }

    console.log('Initializing project %s in %s for engine %s', project_name, dir, engine);
    try {
        var files = fs.readdirSync(dir);
        console.log(error('Directory %s already exists'), dir);
        return;
    } catch (e) {
        // good. the directory doesn't exist
    }

    fs.mkdirsSync(dir);
    fs.mkdirsSync(dir+'/src');
    fs.mkdirsSync(dir+'/build');

    if (engine == 'binocle') {
        fs.copySync(path.join(__dirname, '../../templates/'+engine+'/assets'), dir+'/assets');
    
        processor.processFile(path.join(__dirname, '../../templates/'+engine+'/binocle.json'), dir+'/binocle.json', {project_name:project_name});
        processor.processFile(path.join(__dirname, '../../templates/'+engine+'/CMakeLists.txt'), dir+'/CMakeLists.txt', {project_name:project_name});
        processor.processFile(path.join(__dirname, '../../templates/'+engine+'/src/main.cpp'), dir+'/src/main.cpp', {});
        processor.processFile(path.join(__dirname, '../../templates/'+engine+'/src/MyGame.cpp'), dir+'/src/MyGame.cpp', {});
        processor.processFile(path.join(__dirname, '../../templates/'+engine+'/src/MyGame.hpp'), dir+'/src/MyGame.hpp', {});
        processor.processFile(path.join(__dirname, '../../templates/'+engine+'/src/sys_config.h.cmake'), dir+'/src/sys_config.h.cmake', {});
        if (!this.cloneBinocle(dir)) {
            return;
        }
    } else {
        fs.mkdirsSync(dir+'/src/gameplay');
        fs.copySync(path.join(__dirname, '../../templates/'+engine+'/assets'), dir+'/assets');
    
        processor.processFile(path.join(__dirname, '../../templates/'+engine+'/binocle.json'), dir+'/binocle.json', {project_name:project_name});
        processor.processFile(path.join(__dirname, '../../templates/'+engine+'/CMakeLists.txt'), dir+'/CMakeLists.txt', {project_name:project_name});
        processor.processFile(path.join(__dirname, '../../templates/'+engine+'/src/main.c'), dir+'/src/main.c', {});
        processor.processFile(path.join(__dirname, '../../templates/'+engine+'/src/my_game.c'), dir+'/src/my_game.c', {});
        processor.processFile(path.join(__dirname, '../../templates/'+engine+'/src/my_game.h'), dir+'/src/my_game.h', {});
        processor.processFile(path.join(__dirname, '../../templates/'+engine+'/src/sys_config.h.cmake'), dir+'/src/sys_config.h.cmake', {});
        processor.processFile(path.join(__dirname, '../../templates/'+engine+'/src/gameplay/CMakeLists.txt'), dir+'/src/gameplay/CMakeLists.txt', {});
        processor.processFile(path.join(__dirname, '../../templates/'+engine+'/src/gameplay/my_script.c'), dir+'/src/gameplay/my_script.c', {});
        processor.processFile(path.join(__dirname, '../../templates/'+engine+'/src/gameplay/my_script.h'), dir+'/src/gameplay/my_script.h', {});
        if (!this.cloneBinocleC(dir)) {
            return;
        }
    }

    console.log("Project %s created.", dir);
};

Cmd.prototype.cloneBinocle = function(dir) {
    console.log("Cloning binocle in %s", dir+'/Binocle');
    try {
        var child = spawn('/usr/bin/git', ['clone', 'git@gitlab.altralogica.it:altralogica/binocle.git', dir+'/Binocle', '-b', 'develop', '--depth', '5']);
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

Cmd.prototype.cloneBinocleC = function(dir) {
    console.log("Cloning binocle-c in %s", dir+'/binocle-c');
    try {
        var child = spawn('/usr/bin/git', ['clone', 'git@gitlab.com:Santinelli/binocle-c.git', dir+'/binocle-c', '-b', 'master', '--depth', '5']);
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