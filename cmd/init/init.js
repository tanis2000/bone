var fs = require('fs-extra');
var  chalk = require('chalk');
var hb = require('handlebars');
var spawnSync = require('child_process').spawnSync;
var path = require('path');
var processor = require('../../utils/processor');
var error = chalk.bold.red;
var warning = chalk.yellow;

var Cmd = function() {

};

Cmd.prototype.run = function(app, project_name, dir, engine) {
    if (typeof engine === "undefined" || engine == '') {
        // If not set, we set the default engine to binocle-c
        engine = 'binocle-c';
    }
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

    if (!this.initGit(dir)) {
        console.log(error('Cannot initialize git in directory %s'), dir);
        return;
    }

    fs.mkdirsSync(dir);
    fs.mkdirsSync(dir+'/src');
    fs.mkdirsSync(dir+'/build');

    if (engine == 'binocle') {
        fs.copySync(path.join(__dirname, '../../templates/'+engine+'/assets'), dir+'/assets');
    
        processor.processFile(path.join(__dirname, '../../templates/'+engine+'/binocle.json'), dir+'/binocle.json', {project_name:project_name});
        processor.processFile(path.join(__dirname, '../../templates/'+engine+'/CMakeLists.txt'), dir+'/CMakeLists.txt', {project_name:project_name});
        processor.processFile(path.join(__dirname, '../../templates/'+engine+'/.gitignore.binocle'), dir+'/.gitignore', {});
        processor.processFile(path.join(__dirname, '../../templates/'+engine+'/src/main.cpp'), dir+'/src/main.cpp', {});
        processor.processFile(path.join(__dirname, '../../templates/'+engine+'/src/MyGame.cpp'), dir+'/src/MyGame.cpp', {});
        processor.processFile(path.join(__dirname, '../../templates/'+engine+'/src/MyGame.hpp'), dir+'/src/MyGame.hpp', {});
        processor.processFile(path.join(__dirname, '../../templates/'+engine+'/src/sys_config.h.cmake'), dir+'/src/sys_config.h.cmake', {});

        if (!this.submoduleBinocle(dir)) {
            return;
        }
    } else {
        fs.mkdirsSync(dir+'/src/gameplay');
        fs.copySync(path.join(__dirname, '../../templates/'+engine+'/assets'), dir+'/assets');
    
        processor.processFile(path.join(__dirname, '../../templates/'+engine+'/binocle.json'), dir+'/binocle.json', {project_name:project_name});
        processor.processFile(path.join(__dirname, '../../templates/'+engine+'/CMakeLists.txt'), dir+'/CMakeLists.txt', {project_name:project_name});
        processor.processFile(path.join(__dirname, '../../templates/'+engine+'/.gitignore.binocle'), dir+'/.gitignore', {});
        processor.processFile(path.join(__dirname, '../../templates/'+engine+'/src/main.c'), dir+'/src/main.c', {});
        processor.processFile(path.join(__dirname, '../../templates/'+engine+'/src/my_game.c'), dir+'/src/my_game.c', {});
        processor.processFile(path.join(__dirname, '../../templates/'+engine+'/src/my_game.h'), dir+'/src/my_game.h', {});
        processor.processFile(path.join(__dirname, '../../templates/'+engine+'/src/gameplay/CMakeLists.txt'), dir+'/src/gameplay/CMakeLists.txt', {});
        processor.processFile(path.join(__dirname, '../../templates/'+engine+'/src/gameplay/my_script.c'), dir+'/src/gameplay/my_script.c', {});
        processor.processFile(path.join(__dirname, '../../templates/'+engine+'/src/gameplay/my_script.h'), dir+'/src/gameplay/my_script.h', {});

        if (!this.submoduleBinocleC(dir)) {
            return;
        }
    }

    if (!this.addAllToGit(dir)) {
        console.log(error('Cannot add all files to git in directory %s'), dir);
        return;
    }

    console.log("Project %s created.", dir);
};

Cmd.prototype.cloneBinocle = function(dir) {
    console.log("Cloning binocle in %s", dir+'/Binocle');
    try {
        var child = spawnSync('/usr/bin/git', ['clone', 'git@gitlab.altralogica.it:altralogica/binocle.git', dir+'/Binocle', '-b', 'develop', '--depth', '5']);
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
        var child = spawnSync('/usr/bin/git', ['clone', 'git@gitlab.com:Santinelli/binocle-c.git', dir+'/binocle-c', '-b', 'master', '--depth', '5']);
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

Cmd.prototype.submoduleBinocle = function(dir) {
    console.log("Adding submodule binocle in %s", dir+'/Binocle');
    try {
        var child = spawnSync('/usr/bin/git', ['submodule', 'add', 'git@gitlab.altralogica.it:altralogica/binocle.git', './Binocle', '-b', 'develop', '--depth', '5'], {cwd: dir});
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

Cmd.prototype.submoduleBinocleC = function(dir) {
    console.log("Adding submodule binocle-c in %s", dir+'/binocle-c');
    try {
        var child = spawnSync('/usr/bin/git', ['submodule', 'add', 'git@gitlab.com:Santinelli/binocle-c.git', './binocle-c', '-b', 'master', '--depth', '5'], {cwd: dir});
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

Cmd.prototype.initGit = function(dir) {
    console.log("Initializing git in %s", dir);
    try {
        var child = spawnSync('/usr/bin/git', ['init', dir]);
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

Cmd.prototype.addAllToGit = function(dir) {
    console.log("Adding all files to git in %s", dir);
    try {
        var child = spawnSync('/usr/bin/git', ['add', '*'], {cwd: dir});
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