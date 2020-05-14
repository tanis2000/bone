use std::io::prelude::*;
use std::fs::{File};
use std::path::Path;
use std::process::Command;
use crate::utils::Processor;

#[derive(Default, Builder, Debug)]
#[builder(setter(into))]
pub struct Init {
    app: Option<String>,
    project_name: Option<String>,
    dir: Option<String>,
    engine: Option<String>,
}

impl Init {
    pub fn run(&self) {
        let mut engine = self.engine.clone();
        let _app = self.app.clone().unwrap();
        let project_name = self.project_name.clone().unwrap();
        let dir = self.dir.clone().unwrap();
        match engine {
            None => { 
                engine = Some("binocle-c".to_string());
            },
            _ => {},
        }
        match engine {
            Some(engine) => {
                if engine != "binocle-c" && engine != "binocle" {
                    println!("Error: wrong engine specified: {}", engine);
                    return;
                }
                println!("Initializing project {} in {} for engine {}", project_name, dir, engine);
                let path = Path::new(&dir);
                if path.exists() {
                    println!("Directory {} already exists", dir);
                    return;
                }
                match std::fs::create_dir(path) {
                    Ok(_) => {},
                    Err(e) => {
                        println!("Cannot create directory {} {}", path.display(), e);
                        return;
                    }
                }
                if !self.init_git(&dir) {
                    println!("Cannot initialize git in directory {}", dir);
                    return;
                }
                let src_path = path.join("src");
                match std::fs::create_dir(src_path) {
                    Ok(_) => {},
                    Err(e) => {
                        println!("Cannot create directory {}", e);
                        return;
                    }
                }
                let build_path = path.join("build");
                match std::fs::create_dir(build_path) {
                    Ok(_) => {},
                    Err(e) => {
                        println!("Cannot create directory {}", e);
                        return;
                    }
                }
                let assets_path = path.join("assets");
                match std::fs::create_dir(assets_path.as_path()) {
                    Ok(_) => {},
                    Err(e) => {
                        println!("Cannot create directory {}", e);
                        return;
                    }
                }
                if engine == "binocle" {
                    // Assets
                    Init::copy_file(include_bytes!("../../templates/binocle/assets/wabbit_alpha.png"), assets_path.join("wabbit_alpha.png").as_path());
                    Init::copy_file(include_bytes!("../../templates/binocle/assets/font.fnt"), assets_path.join("font.fnt").as_path());
                    Init::copy_file(include_bytes!("../../templates/binocle/assets/font.png"), assets_path.join("font.png").as_path());

                    // Templates
                    {
                        let context = json!(
                        {
                            "project_name":format!("{}", project_name)
                        });
                        Processor::process(include_bytes!("../../templates/binocle/binocle.json"), path.join("binocle.json").as_path(), &context);
                        Processor::process(include_bytes!("../../templates/binocle/CMakeLists.txt"), path.join("CMakeLists.txt").as_path(), &context);
                        Processor::process(include_bytes!("../../templates/binocle/.gitignore.binocle"), path.join(".gitignore").as_path(), &context);
                        Processor::process(include_bytes!("../../templates/binocle/src/main.cpp"), path.join("src/main.cpp").as_path(), &context);
                        Processor::process(include_bytes!("../../templates/binocle/src/MyGame.cpp"), path.join("src/MyGame.cpp").as_path(), &context);
                        Processor::process(include_bytes!("../../templates/binocle/src/MyGame.hpp"), path.join("src/MyGame.hpp").as_path(), &context);
                        Processor::process(include_bytes!("../../templates/binocle/src/sys_config.h.cmake"), path.join("src/sys_config.h.cmake").as_path(), &context);
                    }

                    if !self.submodule_binocle(&dir) {
                        println!("Error: cannot initialize binocle submodule");
                        return;
                    }
                } else {
                    // Folders
                    let gameplay_path = path.join("src").join("gameplay");
                    match std::fs::create_dir(gameplay_path.as_path()) {
                        Ok(_) => {},
                        Err(e) => {
                            println!("Cannot create directory {}", e);
                            return;
                        }
                    }

                    // Assets
                    Init::copy_file(include_bytes!("../../templates/binocle-c/assets/default.frag"), assets_path.join("default.frag").as_path());
                    Init::copy_file(include_bytes!("../../templates/binocle-c/assets/default.vert"), assets_path.join("default.vert").as_path());
                    Init::copy_file(include_bytes!("../../templates/binocle-c/assets/wabbit_alpha.png"), assets_path.join("wabbit_alpha.png").as_path());
                    Init::copy_file(include_bytes!("../../templates/binocle-c/assets/font.fnt"), assets_path.join("font.fnt").as_path());
                    Init::copy_file(include_bytes!("../../templates/binocle-c/assets/font.png"), assets_path.join("font.png").as_path());

                    // Templates
                    {
                        let context = json!(
                        {
                            "project_name":format!("{}", project_name)
                        });
                        Processor::process(include_bytes!("../../templates/binocle-c/binocle.json"), path.join("binocle.json").as_path(), &context);
                        Processor::process(include_bytes!("../../templates/binocle-c/CMakeLists.txt"), path.join("CMakeLists.txt").as_path(), &context);
                        Processor::process(include_bytes!("../../templates/binocle-c/.gitignore.binocle"), path.join(".gitignore").as_path(), &context);
                        Processor::process(include_bytes!("../../templates/binocle-c/src/main.c"), path.join("src/main.c").as_path(), &context);
                        Processor::process(include_bytes!("../../templates/binocle-c/src/my_game.c"), path.join("src/my_game.c").as_path(), &context);
                        Processor::process(include_bytes!("../../templates/binocle-c/src/my_game.h"), path.join("src/my_game.h").as_path(), &context);
                        Processor::process(include_bytes!("../../templates/binocle-c/src/gameplay/CMakeLists.txt"), path.join("src/gameplay/CMakeLists.txt").as_path(), &context);
                        Processor::process(include_bytes!("../../templates/binocle-c/src/gameplay/my_script.c"), path.join("src/gameplay/my_script.c").as_path(), &context);
                        Processor::process(include_bytes!("../../templates/binocle-c/src/gameplay/my_script.h"), path.join("src/gameplay/my_script.h").as_path(), &context);
                    }

                    if !self.submodule_binocle_c(&dir) {
                        println!("Error: cannot initialize binocle-c submodule");
                        return;
                    }
                }

                if !self.add_all_to_git(&dir) {
                    println!("Cannot add all files to git in directory {}", dir);
                    return;
                }

                println!("Project {} created", project_name);
            },
            None => {},
        }
    }

    pub fn init_git(&self, dir: &String) -> bool {
        println!("Initializing git in {}", dir);
        let output = Command::new("/usr/bin/git")
        .args(&["init", dir])
        .output()
        .expect("Failed to initialize git repository");
        println!("{}", String::from_utf8(output.stdout).unwrap());
        true
    }

    pub fn add_all_to_git(&self, dir: &String) -> bool {
        println!("Adding all files to git in {}", dir);
        {
            let output = Command::new("/usr/bin/git")
            .args(&["add", "*"])
            .current_dir(dir)
            .output()
            .expect("Failed to add files to git repository");
            println!("{}", String::from_utf8(output.stdout).unwrap());
        }
        {
            let output = Command::new("/usr/bin/git")
            .args(&["commit", "-m", "\"First import\""])
            .current_dir(dir)
            .output()
            .expect("Failed to commit files to git repository");
            println!("{}", String::from_utf8(output.stdout).unwrap());
        }
        true
    }

    pub fn submodule_binocle_c(&self, dir: &String) -> bool {
        println!("Adding submodule binocle-c in {}", dir);
        let output = Command::new("/usr/bin/git")
        .args(&["submodule", "add", "https://github.com/tanis2000/binocle-c.git", "./binocle-c", "-b", "master", "--depth", "5"])
        .current_dir(dir)
        .output()
        .expect("Failed to initialize binocle-c submodule");
        println!("{}", String::from_utf8(output.stdout).unwrap());
        true
    }

    pub fn submodule_binocle(&self, dir: &String) -> bool {
        println!("Adding submodule binocle in {}", dir);
            let output = Command::new("/usr/bin/git")
            .args(&["submodule", "add", "git@gitlab.altralogica.it:altralogica/binocle.git", "./Binocle", "-b", "develop", "--depth", "5"])
            .current_dir(dir)
            .output()
            .expect("Failed to initialize binocle submodule");
            println!("{}", String::from_utf8(output.stdout).unwrap());
        true
    }

    pub fn copy_file(bytes: &[u8], path: &Path) {
        let file = File::create(path);
        match file {
            Ok(mut file) => {
                match file.write_all(bytes) {
                    Ok(_) => {},
                    Err(e) => {
                        println!("Error: cannot write to file {} {}", path.to_string_lossy(), e.to_string());
                    }
                }
            },
            Err(e) => {
                println!("Error: cannot create file {} {}", path.to_string_lossy(), e.to_string());
            }
        }
    }
}