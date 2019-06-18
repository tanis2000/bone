use serde_json::{Value};
use std::io::prelude::*;
use std::fs::{File};
use std::path::Path;
use crate::utils::Processor;

#[derive(Default, Builder, Debug)]
#[builder(setter(into))]
pub struct Update {
    app: Option<String>,
    dir: Option<String>,
}

impl Update {
    pub fn run(&self) {
        let _app = self.app.clone().unwrap();
        let dir = self.dir.clone().unwrap();

        println!("Updating project in {}", dir);
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

                        let context = json!({});

                        if engine == "binocle" {
                            Processor::process(include_bytes!("../../templates/binocle/CMakeLists.txt"), path.join("CMakeLists.txt").as_path(), &context);
                        } else {
                            Processor::process(include_bytes!("../../templates/binocle-c/CMakeLists.txt"), path.join("CMakeLists.txt").as_path(), &context);
                        }
                        println!("Project {} updated", dir);
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
}