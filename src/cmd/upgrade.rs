use serde_json::{Result, Value};
use std::io;
use std::io::prelude::*;
use std::fs::{self, DirEntry, File};
use std::path::Path;
use std::process::Command;
use crate::utils::Processor;

#[derive(Default, Builder, Debug)]
#[builder(setter(into))]
pub struct Upgrade {
    app: Option<String>,
    dir: Option<String>,
}

impl Upgrade {
    pub fn run(&self) {
        let app = self.app.clone().unwrap();
        let dir = self.dir.clone().unwrap();

        println!("Upgrading project in {}", dir);
        let path = Path::new(&dir);
        let entries = path.read_dir();
        match entries {
            Ok(entries) => {
                if entries.count() == 0 {
                    println!("Error: project directory is empty. Cannot update.");
                    return;
                }
            },
            Err(e) => {
                println!("Error: cannot read project folder, maybe it doesn't exist. {}", e.to_string());
                return;
            }
        }

        let project_path = path.join("binocle.json");
        println!("Looking for project file {}", project_path.to_string_lossy());
        let project_file = File::open(project_path);
        match project_file {
            Ok(mut project_file) => {
                let mut project_content = String::new();
                let read_size = project_file.read_to_string(&mut project_content);
                match read_size {
                    Ok(_read_size) => {
                        println!("{}", project_content);
                        let project_json: Value = serde_json::from_str(project_content.as_str()).unwrap();
                        let engine = &project_json["project"]["engine"];
                        println!("{}", engine);
                        if engine != "binocle-c" && engine != "binocle" {
                            println!("Error: wrong engine specified: {}", engine);
                            return;
                        }

                        if engine == "binocle" {
                            if !self.upgrade_binocle(&dir) {
                                println!("Error: cannot upgrade binocle in folder {}", dir);
                                return;
                            }
                        } else {
                            if !self.upgrade_binocle_c(&dir) {
                                println!("Error: cannot upgrade binocle-c in folder {}", dir);
                                return;
                            }
                        }
                        println!("Project {} upgraded", dir);
                    },
                    Err(e) => {
                        println!("Error: cannot read project file. {}", e.to_string());
                        return;
                    },
                }
            },
            Err(e) => {
                println!("Error: cannot open project file. {}", e.to_string());
                return;
            }
        }
    }

    pub fn upgrade_binocle(&self, dir: &String) -> bool {
        println!("Upgrading binocle in {}", dir);
        let output = Command::new("/usr/bin/git")
        .args(&["-C", "./Binocle", "pull", "origin"])
        .current_dir(dir)
        .output()
        .expect("Failed to upgrade binocle submodule");
        println!("{}", String::from_utf8(output.stdout).unwrap());
        true
    }

    pub fn upgrade_binocle_c(&self, dir: &String) -> bool {
        println!("Upgrading binocle-c in {}", dir);
        let output = Command::new("/usr/bin/git")
        .args(&["-C", "./binocle-c", "pull", "origin"])
        .current_dir(dir)
        .output()
        .expect("Failed to upgrade binocle-c submodule");
        println!("{}", String::from_utf8(output.stdout).unwrap());
        true
    }

}