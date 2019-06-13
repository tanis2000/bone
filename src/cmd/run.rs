use serde_json::{Result, Value};
use std::io;
use std::io::prelude::*;
use std::fs::{self, DirEntry, File};
use std::path::Path;
use std::process::{Command, Stdio};
use crate::utils::Processor;
use crate::utils::Target;

#[derive(Default, Builder, Debug)]
#[builder(setter(into))]
pub struct Run {
    app: Option<String>,
    dir: Option<String>,
    target: Option<String>,
}

impl Run {
    pub fn run(&self) {
        let app = self.app.clone().unwrap();
        let dir = self.dir.clone().unwrap();
        let target_ss = self.target.clone().unwrap().clone();
        let target_s = target_ss.as_str();
        let mut target = Target::Unknown;

        match target_s {
            "mac" => {
                target = Target::Mac
            },
            "windows" => {
                target = Target::Windows
            },
            "linux" => {
                target = Target::Linux
            },
            "ios" => {
                target = Target::Ios
            },
            "android" => {
                target = Target::Android
            },
            "web" => {
                target = Target::Web
            },
            _ => {
                println!("Unsupported target {}", target_s);
                return;
            }
        }


        println!("Running project in {} for target platform {}", dir, target.to_string());

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

        if !self.run_project(&dir, &target) {
            println!("Error: cannot run project");
            return;
        }

        println!("Project run.");
    }

    pub fn run_project(&self, dir: &String, target: &Target) -> bool {
        let path = Path::new(&dir);
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
                        if engine != "binocle-c" && engine != "binocle" {
                            println!("Error: wrong engine specified: {}", engine);
                            return false;
                        }
                        let ios_development_team_id = &project_json["project"]["ios_development_team_id"];
                        let team_id = format!("IOS_DEVELOPMENT_TEAM_ID={}", ios_development_team_id);
                        let team_id_s = team_id.as_str();
                        let project_name = &project_json["project"]["name"].as_str().unwrap();
                        match target {
                            Mac => {
                                let command = "open".to_string();
                                let filename = format!("{}.app", project_name);
                                let app_path = Path::new(&filename);
                                let app_path_s = app_path.to_str().unwrap();
                                let app_name = project_name.to_string();
                                let app_name_s = app_name.as_str();
                                let parameters = [app_name_s];
                                let cwd = path.join("build").join(target.to_string()).join("src").join(&filename).join("Contents").join("MacOS");
                                let cwd_s = cwd.to_str().unwrap().to_string();
                                println!("Running application {} in dir {}", project_name, cwd_s);
                                self.execute_command(&command, &parameters, &cwd_s);
                            },
                            Windows => {
                                println!("Do not know how to run for this target. Please submit a PR if you know what to do");
                                return false;
                            },
                            Linux => {
                                println!("Do not know how to run for this target. Please submit a PR if you know what to do");
                                return false;
                            },
                            Ios => {
                            },
                            Android => {
                                println!("Do not know how to run for this target. Please submit a PR if you know what to do");
                                return false;
                            },
                            Web => {
                                println!("Do not know how to run for this target. Please submit a PR if you know what to do");
                                return false;
                            }
                        }
                    },
                    Err(e) => {
                        println!("Error: cannot read project file. {}", e.to_string());
                        return false;
                    },
                }
            },
            Err(e) => {
                println!("Error: cannot open project file. {}", e.to_string());
                return false;
            }
        }
        true
    }

    pub fn execute_command(&self, command: &String, parameters: &[&str], cwd: &String) {
        let output = Command::new(command)
        .args(parameters)
        .current_dir(cwd)
        .output()
        .expect("Failed to run command");
        println!("{}", String::from_utf8(output.stdout).unwrap());
    }


}